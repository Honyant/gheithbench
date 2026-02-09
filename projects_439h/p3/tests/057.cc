#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "promise.h"

#define R 5
#define C 5

int A[R][C] = { { 3, 7, 2, 10 },
                { 5, 1, 8, 4 },
                { 9, 6, 11, 3 },
                { 2, 14, 7, 5 } };

int B[R][C] = { {4, 8, 6, 2},
                {1, 3, 9, 7},
                {12, 5, 0, 11},
                {3, 2, 13, 4} };

int P[R][C] = { {0, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0} };

/*
|  73   75  211  117 |
| 129   91   91  121 |
| 183  151  147  193 |
| 121  103  203  199 |
*/

Atomic<int> num_threads{R*C};

void kernelMain(void) {

    /*
    Perform matrix multiplication with aid of promises
    */

    for(int r = 0; r < R; r++){
        for(int c = 0; c < C; c++){
            thread([r, c]{
                int sum = 0;
                for(int i = 0; i < C; i++){
                    sum += A[r][i] * B[i][c];
                }
                P[r][c] = sum;
                num_threads.fetch_add(-1);
                while(true) {} // must use interrupts to get out of loop
            });
        }
    }

    while(num_threads.get() > 0) {}

    for(int r = 0; r < R; r++){
        Debug::printf("*** %d %d %d %d\n", P[r][0], P[r][1], P[r][2], P[r][3]);
    }

}
