#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

//This test case uses parallel processing to do matrix multiplication efficiently.


// Computes the dot product for a specific cell in the result matrix, A and B are the input matrcies

void computeDotProduct(int row, int col, int M, int** A, int** B, Promise<int>* promise, Barrier* barrier) {
    int sum = 0;
    for (int k = 0; k < M; k++) {
        sum += A[row][k] * B[k][col];
    }
    // send the result thru a promise
    promise->set(sum);
    barrier->sync();
}

// this thread calls get for all the promises
void collectResults(int N, int P, Promise<int>** promises, int** C, Barrier* barrier) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            Debug::printf("*** Waiting for promise at C[%d][%d]\n", i, j);
            C[i][j] = promises[i * P + j]->get();  // This should block until the promise is fulfilled
            Debug::printf("*** Received value for C[%d][%d]: %d\n", i, j, C[i][j]);
        }
    }
    barrier->sync();
}

// matmul
void matrixMultiplyWithPromises(int N, int M, int P, int** A, int** B, int** C) {
    int numThreads = N * P;
    auto barrier = new Barrier(numThreads + 2); 

    // array of promises for each cell in the result matrix
    Promise<int>** promises = new Promise<int>*[numThreads];

    // start the result-collecting thread first to test blocking behavior
    thread([N, P, promises, C, barrier] {
        collectResults(N, P, promises, C, barrier);
        
        // cleanup promise arr
        for (int i = 0; i < N * P; i++) {
            delete promises[i];
        }
        delete[] promises;
    });

    // create a thread for each cell in the result matrix
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            promises[i * P + j] = new Promise<int>();

            // Each thread computes the value for one cell (i, j)
            thread([i, j, M, A, B, P, promises, barrier] {
                computeDotProduct(i, j, M, A, B, promises[i * P + j], barrier);
            });
        }
    }

    barrier->sync();
}

void kernelMain() {
    //dimensions of input matrices
    int N = 4, M = 4, P = 4; // Dimensions: A[N][M], B[M][P], the result matrix: C[N][P]

    int** A = new int*[N];
    int** B = new int*[M];
    int** C = new int*[N];
    for (int i = 0; i < N; i++) A[i] = new int[M];
    for (int i = 0; i < M; i++) B[i] = new int[P];
    for (int i = 0; i < N; i++) C[i] = new int[P];

    // create matrix A with some random values
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            A[i][j] = i + j + 1; 
        }
    }

    // create matrix B with some random values
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < P; j++) {
            B[i][j] = i * j + 2; 
        }
    }

    matrixMultiplyWithPromises(N, M, P, A, B, C);

    // delete all the allocated matrices
    for (int i = 0; i < N; i++) delete[] A[i];
    for (int i = 0; i < M; i++) delete[] B[i];
    for (int i = 0; i < N; i++) delete[] C[i];
    delete[] A;
    delete[] B;
    delete[] C;
}