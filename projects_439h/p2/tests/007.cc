#include "debug.h"
#include "threads.h"
#include "promise.h"
#include "barrier.h"
#include "random.h"

/* You have a probability transformation represented by a randomly generated stochastic matrix of rational numbers.
 * However, I have decided to transform your probability transformation.
 * The transformation I will apply is also a randomly generated stochastic matrix of rational numbers.
 * Your job is to confirm that this new probability transformation is still valid.
 * To do so, you will multiply your transformation by my transformation, then check that the result is stochastic.
 * Stochastic matrices are those for which right-multiplication by any real vector preserves its 1-norm (the sum of its elements).
 * Additionally, if a real vector has only nonnegative entries, left-multiplication by a stochastic matrix must yield another real vector with only nonnegative entries.
 * To check that a matrix is stochastic, verify that all its entries are nonnegative and that each column's entries sum to 1. 
 */

/* This test case ensures that your threads, promises, and barriers work and interact correctly and efficiently.
 * You will likely either (1) pass, (2) fail the distribution check due to a race condition, (3) deadlock and time out, or (4) run out of memory.
 * If every thread the program creates coexisted at once, the program would run out of memory.
 * Hence, freeing TCBs is necessary, as is implementing a scheduling policy that cycles through nearly-done threads (FIFO with proper yielding works).
 */

#define MATRIX_SIZE 101 // 100 doesn't work here, and neither does 102. The first person to explain why this is the case will win the Easter egg hunt.

// Euclid's algorithm for computing the greatest common divisor of two nonnegative integers
uint32_t euclid(uint32_t a, uint32_t b) {
    if (b == 0) return a;
    return euclid(b, a % b);
}

// useful for reducing rational numbers
uint32_t gcd(uint32_t num, uint32_t den) {
    if (num > den) {
        return euclid(num, den);
    } else {
        return euclid(den, num);
    }
}

// nonnegative rational number
typedef struct Q {
    uint32_t num;
    uint32_t den;
    Q(uint32_t num, uint32_t den) {
        ASSERT(den != 0);
        uint32_t div = gcd(num, den);
        this->num = num/div;
        this->den = den/div;
    }
    Q operator*(Q two) {
        return Q(num * two.num, den * two.den);
    }
    Q operator+(Q two) {
        return Q(num * two.den + den * two.num, den * two.den);
    }
    void operator+=(Q two) {
        *this = Q(num * two.den + den * two.num, den * two.den);
    }
    bool operator==(Q two) {
        return num * two.den == den * two.num;
    }
} Q;

void kernelMain(void) {

    Debug::printf("*** Beginning computational process.\n");
    
    const uint32_t column_sum = MATRIX_SIZE * MATRIX_SIZE;

    // why can't we use "new" for the promises or the barriers?
    Barrier* allocationBarrier = (Barrier*) malloc(sizeof(Barrier));
    Barrier* initializationBarrier = (Barrier*) malloc(sizeof(Barrier));
    Barrier* verificationBarrier = (Barrier*) malloc(sizeof(Barrier));
    Barrier* deallocationBarrier = (Barrier*) malloc(sizeof(Barrier));
    Promise<bool> *done = (Promise<bool>*) malloc(sizeof(Promise<bool>)); 
    Promise<bool> *results = (Promise<bool>*) malloc(kConfig.totalProcs * sizeof(Promise<bool>));
    Promise<bool> *column_equals_1 = (Promise<bool>*) malloc(MATRIX_SIZE * sizeof(Promise<bool>));

    for (uint32_t i = 0; i < kConfig.totalProcs; i++) {
        results[i] = Promise<bool>{};
    }
    for (uint32_t i = 0; i < MATRIX_SIZE; i++) {
        column_equals_1[i] = Promise<bool>{};
    }
    *done = Promise<bool>{}; 

    Debug::printf("*** Variables initialized.\n");

    thread([results, &done]{
            // should not make much progress until the the program nears its end
            bool result = true;
            for (uint32_t i = 0; i < kConfig.totalProcs; i++) {
                if (!results[i].get()) {
                    result = false;
                    break;
                }
            }
            if (result) { 
                Debug::printf("*** The probability transformation is valid!\n");
            } else {
                Debug::printf("*** The probability transformation is not valid.\n");
            }
            done->set(true);
        });

    for (uint32_t i = 0; i < kConfig.totalProcs; i++) {
        // if your promises spin, these threads will almost certainly occupy all the cores, causing deadlock by thread starvation
        thread([results, i, column_equals_1]{
            bool result = true;
            for (uint32_t col = 0; col < MATRIX_SIZE; col++) {
                if (!column_equals_1[col].get()) {
                    result = false;
                    break;
                }
            }
            results[i].set(result);
        }
        );
    }

    yield(); // helps check that previous threads don't spin
    Debug::printf("*** Verifying and printing threads spawned.\n");

    Q** yours = (Q**) malloc(MATRIX_SIZE * sizeof(Q*));
    Q** mine = (Q**) malloc(MATRIX_SIZE * sizeof(Q*));
    Q** product = (Q**) malloc(MATRIX_SIZE * sizeof(Q*));

    *allocationBarrier = Barrier{3 * MATRIX_SIZE + 1}; 
    for (uint32_t row = 0; row < MATRIX_SIZE; row++) {
        thread([yours, row, allocationBarrier]{
            yours[row] = (Q*) malloc(MATRIX_SIZE * sizeof(Q));
            allocationBarrier->sync();
        });
        thread([mine, row, allocationBarrier]{
            mine[row] = (Q*) malloc(MATRIX_SIZE * sizeof(Q));
            allocationBarrier->sync();
        });
        thread([product, row, allocationBarrier]{
            product[row] = (Q*) malloc(MATRIX_SIZE * sizeof(Q));
            allocationBarrier->sync();
        });
    }
    allocationBarrier->sync();

    yield(); // should help clean up old threads
    Debug::printf("*** Memory allocated.\n");

    *initializationBarrier = Barrier{2 * MATRIX_SIZE + 1};
    for (uint32_t col = 0; col < MATRIX_SIZE; col++) {
        thread([column_sum, col, yours, &initializationBarrier] {
            uint32_t col_sum = column_sum;
            Random random{col};
            for (uint32_t row = 0; row < MATRIX_SIZE - 1; row++) {
                uint32_t num = random.next() % (col_sum / (MATRIX_SIZE - row));
                col_sum -= num;
                yours[row][col] = Q(num, column_sum);
            }
            yours[MATRIX_SIZE - 1][col] = Q(col_sum, column_sum);
            initializationBarrier->sync();
        });
        thread([column_sum, col, mine, &initializationBarrier] {
            uint32_t col_sum = column_sum;
            Random random{col};
            for (uint32_t row = 0; row < MATRIX_SIZE - 1; row++) {
                uint32_t num = random.next() % (col_sum / (MATRIX_SIZE - row));
                col_sum -= num;
                mine[row][col] = Q(num, column_sum);
            }
            mine[MATRIX_SIZE - 1][col] = Q(col_sum, column_sum);
            initializationBarrier->sync();
        });
    }
    initializationBarrier->sync();

    yield(); // should help clean up old threads
    Debug::printf("*** Matrices generated.\n");

    for (uint32_t col = 0; col < MATRIX_SIZE; col++) {
        Barrier* multiplicationBarrier = (Barrier*) malloc(sizeof(Barrier));
        *multiplicationBarrier = Barrier{MATRIX_SIZE + 1};
        for (uint32_t row = 0; row < MATRIX_SIZE; row++) {
            thread([col, row, yours, mine, product, multiplicationBarrier]{
                product[row][col] = Q(0, 1);
                for (uint32_t i = 0; i < MATRIX_SIZE; i++) {
                    product[row][col] += mine[row][i] * yours[i][col];
                }
                multiplicationBarrier->sync();
            });
        }
        multiplicationBarrier->sync();
        yield(); // should help clean up old threads
    }
    Debug::printf("*** Multiplication performed.\n");

    *verificationBarrier = Barrier{MATRIX_SIZE + 1};
    for (uint32_t col = 0; col < MATRIX_SIZE; col++) {
        thread([col, product, column_equals_1, verificationBarrier] {
            Q sum{0, 1};
            for (uint32_t row = 0; row < MATRIX_SIZE; row++) {
                sum += product[row][col];
            }
            column_equals_1[col].set(sum == Q(1, 1));
            verificationBarrier->sync();
        });
    }
    verificationBarrier->sync();

    yield(); // should help clean up old threads

    if (!done->get()) {
        // ensures that print occurs before program exits
        Debug::printf("*** Promise either not honored or improperly set.\n");
    }

    yield(); // should help clean up old threads
    Debug::printf("*** Stochasticity checks performed.\n");

    *deallocationBarrier = Barrier{3 * MATRIX_SIZE + 1};
    for (uint32_t row = 0; row < MATRIX_SIZE; row++) {
        thread([yours, row, deallocationBarrier]{
            free(yours[row]);
            deallocationBarrier->sync();
        });
        thread([mine, row, deallocationBarrier]{
            free(mine[row]);
            deallocationBarrier->sync();
        });
        thread([product, row, deallocationBarrier]{
            free(product[row]);
            deallocationBarrier->sync();
        });
    }
    deallocationBarrier->sync();

    yield(); // should help clean up old threads
    Debug::printf("*** Memory deallocated.\n");

    free(yours);
    free(mine);
    free(product);
    Debug::printf("*** Cleanup complete. Exiting.\n");
}

