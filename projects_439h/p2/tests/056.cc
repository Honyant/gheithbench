#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "promise.h"

#define R 4
#define C 4

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

void kernelMain(void) {

    /*
    Perform matrix multiplication with aid of promises
    */

    Promise<int>* promises[R][C];

    for(int r = 0; r < R; r++){
        for(int c = 0; c < C; c++){
            promises[r][c] = new Promise<int>();
            auto p = promises[r][c];
            thread([p, r, c]{
                int sum = 0;
                for(int i = 0; i < C; i++){
                    sum += A[r][i] * B[i][c];
                }
                p->set(sum);
            });
        }
    }

    for(int r = 0; r < R; r++){
        Debug::printf("*** %d %d %d %d\n", promises[r][0]->get(), promises[r][1]->get(), promises[r][2]->get(), promises[r][3]->get());
    }

    for(int r = 0; r < R; r++){
        for(int c = 0; c < C; c++){
            delete promises[r][c];
        }
    }

}
