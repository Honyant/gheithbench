#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

// A TEST CASE THAT WILL CONSIST OF 3 PARTS

// PART 1: SIMPLE TEST OF BARRIER AND BASIC PROMISE FUNCTIONALITY
// PART 2: MULTYPLING A MATRIX WITH A SCALAR WITH MANY THREADS
// PART 3: TRANSPOSING A MATRIX AFTER PART 2, AND WAITING FOR PART 2 TO COMPELTE USING A PROMISE

Atomic<int> cntForTestFour{0};

/* Called by one core */
void kernelMain(void) {

    // a test that will test barriers and promises in a very simple way.
    // each iteration of the loop should create a new thread which immdietly sets and then
    // gets from a promise that is only available in that iteration of the loop.
    // this is a simple test for the basic implementation
    Debug::printf("*** ------------------------------\n");
    Debug::printf("*** START TEST SECTION ONE \n");
    int NUMBER_OF_ITERATIONS = 30;
    for (int iterationNumber = 0; iterationNumber < NUMBER_OF_ITERATIONS; iterationNumber++) {
        // prints out the iteration number
        auto iterationBarrier = new Barrier(2);
        thread([iterationBarrier, iterationNumber]{
            auto simplePromise = new Promise<int>();

            simplePromise->set(iterationNumber);
            Debug::printf("*** iteration number: %d\n", simplePromise->get());

            // forces thread to complete before new iteration
            iterationBarrier->sync();
        });

        // all threads in this iteration should complete
        iterationBarrier->sync();
    }
    Debug::printf("*** END TEST SECTION ONE \n\n");

    // -----------------------------------------------------------------------------------

    // the initial state of the matrix
    int matrix[4][4] = {{1, 2, 3, 4},
                        {5, 6, 7, 8},
                        {9, 10, 11, 12},
                        {13, 14, 15, 16}};

    // test section three should be gated by a promise
    // this means that it should really only start when the multiplication of test 2 (later in the code) has finished
    // test 3 takes the multiplied matrix form test 2, finds its transpose using threads, and prints it out
    auto hasFinishedMult = new Promise<int>();
    auto hasFinishedTranspose = new Promise<int>();
    thread([hasFinishedMult, hasFinishedTranspose, &matrix]{
        // block & wait for the promise to resolve
       hasFinishedMult->get();
       Debug::printf("*** ------------------------------\n");
       Debug::printf("*** START TEST SECTION THREE\n");

       // find the transpose of the matrix and put it into ans: one element per thread
       int ans[4][4];
       auto matrixTransposeBarrier = new Barrier(17);
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                thread([&ans, row, col, &matrix, matrixTransposeBarrier]{
                    ans[row][col] = matrix[col][row];

                    matrixTransposeBarrier->sync();
                });
            }
        }
        matrixTransposeBarrier->sync();

        // print out the resultant matrix
        for (int row = 0; row < 4; row++) {
            Debug::printf("*** ");
            for (int col = 0; col < 4; col++) {
                Debug::printf("%d ", ans[row][col]);
            }
            Debug::printf("\n");
        }

       Debug::printf("*** END TEST SECTION THREE \n\n");
       hasFinishedTranspose->set(true);
    });

    // -----------------------------------------------------------------------------------

    // this is a test that will test multiplying your matrix with a scalar
    // all the multiplications of each item in the matrix will happen in its own individual thread
    Debug::printf("*** ------------------------------\n");
    Debug::printf("*** START TEST SECTION TWO \n");

    // scalar we are multyplying with
    int SCALAR = 10;

    // 4 x 4 + 1 = 17 barriers
    auto matrixMultScalarBarrier = new Barrier(17);

    // create a thread for each elemeent in the matrix and multiply :)
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            thread([row, col, &matrix, matrixMultScalarBarrier, SCALAR]{
                matrix[row][col] = SCALAR * matrix[row][col];

                matrixMultScalarBarrier->sync();
            });
        }
    }
    matrixMultScalarBarrier->sync();

    // print out the resultant matrix
    for (int row = 0; row < 4; row++) {
        Debug::printf("*** ");
        for (int col = 0; col < 4; col++) {
            Debug::printf("%d ", matrix[row][col]);
        }
        Debug::printf("\n");
    }

    Debug::printf("*** END TEST SECTION TWO \n\n");

    // set the promise to start test 3
    hasFinishedMult->set(true);
    hasFinishedTranspose->get();

    Debug::printf("*** YOU REACHED THE END, CONGRATS! :D \n");
}