#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

/**
 * This test will perform a very common application of parallel programming: matrix multiplication!
 * 
 * mat1 will contain numbers in order left to right, top to bottom, starting from 0
 * ex if mat1 was of size 3 it would look like:
 *   0 1 2
 *   3 4 5
 *   6 7 8
 *
 * mat2 will be 2 * the identity matrix
 * ex if mat2 was of size 3:
 *   2 0 0
 *   0 2 0
 *   0 0 2
 * 
 * The resulting multiplication should be the same as 2 * mat1!
 */

#define MATRIX_SIZE 8

void kernelMain(void) {
    int mat1[MATRIX_SIZE][MATRIX_SIZE]; // mat1 will be filled with increasing numbers
    int mat2[MATRIX_SIZE][MATRIX_SIZE]; // mat2 will double the values in mat1 (2 * identity matrix)
    int mat3[MATRIX_SIZE][MATRIX_SIZE]; /// mat3 will store the product 

    // Let's initialize both matrices using barriers! 
    // Each thread will be responsible for initializing a single row of the matrices.
    Debug::printf("*** Initializing matrices using barriers...\n");
    Barrier b(MATRIX_SIZE + 1);
    for (int row = 0; row < MATRIX_SIZE; row++) {
        thread([&mat1, &mat2, row, &b] {
            for (int i = 0; i < MATRIX_SIZE; i++) {
                mat1[row][i] = row * MATRIX_SIZE + i;
                mat2[row][i] = row == i ? 2 : 0;
            }
            b.sync();
        });
    } 

    // wait for all rows to be initialized
    b.sync();

    // some quick sanity checks
    Debug::printf("*** Testing...\n");
    Debug::printf("*** %d\n", mat1[0][0]); // 0
    Debug::printf("*** %d\n", mat1[MATRIX_SIZE - 1][MATRIX_SIZE - 1]); //  MATRIX_SIZE * MATRIX_SIZE - 1
    Debug::printf("*** %d\n", mat2[0][0]); // 2
    Debug::printf("*** %d\n", mat2[MATRIX_SIZE - 1][MATRIX_SIZE - 1]); // 2 

    Debug::printf("*** Multiplying matrices using promises...\n");
    // Ok time to multiply, but now with promises!
    // Each thread will be responsible for a value in the final matrix. (dot product)
    Promise<int> promises[MATRIX_SIZE][MATRIX_SIZE];
    for (int y = 0; y < MATRIX_SIZE; y++) {
        for (int x = 0; x < MATRIX_SIZE; x++) {
            thread([&promises, &mat1, &mat2, x, y] {
                int out = 0;
                for (int i = 0; i < MATRIX_SIZE; i++) {
                    out += mat1[y][i] * mat2[i][x];
                }
                promises[y][x].set(out); 
            });
        }
    }

    for (int y = 0; y < MATRIX_SIZE; y++) {
        for (int x = 0; x < MATRIX_SIZE; x++) {
            mat3[y][x] = promises[y][x].get();
        }
    }

    // checking if mat3 = 2*mat1
    Debug::printf("*** Testing...\n");
    for (int y = 0; y < MATRIX_SIZE; y++) {
        for (int x = 0; x < MATRIX_SIZE; x++) {
            if (mat3[y][x] != 2 * mat1[y][x]) {
                Debug::printf("*** MULTIPLICATION FAILED!\n");
                Debug::shutdown();
            }
        }
    }
    Debug::printf("*** YAY!!! FINISHED!!!\n");
}

