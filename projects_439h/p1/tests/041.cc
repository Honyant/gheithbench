#include "debug.h"
#include "critical.h"
#include "smp.h"

static Barrier* barrier1 = new Barrier(4);
static Barrier* barrier2 = new Barrier(4);

//let's multiply two matrices and then take the resulting determinant with our four cores!

//first, we define the first matrix:
//A = [  3   7
//       6   2  ]
static uint32_t a11 = 3;
static uint32_t a12 = 7;
static uint32_t a21 = 6;
static uint32_t a22 = 2;

//next, we define the second matrix:
//B = [  8   4
//       9   6  ]
static uint32_t b11 = 8;
static uint32_t b12 = 4;
static uint32_t b21 = 9;
static uint32_t b22 = 6;

//let's initialize our answer matrix:
static uint32_t ab11 = 0;
static uint32_t ab12 = 0;
static uint32_t ab21 = 0;
static uint32_t ab22 = 0;

//let's initialize our diagonal products (used for determinant calculation):
static uint32_t d1 = 0;
static uint32_t d2 = 0;

//these are counter variables used to make outputs sequential:
static uint32_t productCalculation = 0;
static uint32_t determinantCalculation = 0;

//computes top left, top right, bottom left, and then bottom right (in that order) of the product matrix
//prints the output of each step to test that correct sequence of the global counter variable is maintained via critical section
//the core that enters is assigned to the next step in the process above
//global counter variable productCalculation is updated by the last core who entered this function to keep process sequential
void computeNextResult(void) {
    //this print statement should increment from 0 to 3
    //tests that only one core is in the critical section at a time so that the counter variable's value is correct for each core
    Debug::printf("*** A core is running step #%d\n", productCalculation);

    //steps should be performed in correct order based on incrementing of global counter variable by cores in a critical section
    if (productCalculation == 0) {
        ab11 = (a11 * b11) + (a12 * b21);

        //the other three members of the answer matrix should not have modified yet
        if (ab12 != 0 or ab21 != 0 or ab22 != 0) {
            Debug::printf("*** FAIL: WRONG EXECUTION ORDER \n");
        }

        Debug::printf("*** AB (1, 1) = %d\n", ab11);
    } else if (productCalculation == 1) {
        ab12 = (a11 * b12) + (a12 * b22);

        //first member of answer matrix is the only one already modified
        if (ab11 == 0 or ab21 != 0 or ab22 != 0) {
            Debug::printf("*** FAIL: WRONG EXECUTION ORDER \n");
        }

        Debug::printf("*** AB (1, 2) = %d\n", ab12);
    } else if (productCalculation == 2) {
        ab21 = (a21 * b11) + (a22 * b21);

        //first two members of answer matrix are the only ones already modified
        if (ab11 == 0 or ab12 == 0 or ab22 != 0) {
            Debug::printf("*** FAIL: WRONG EXECUTION ORDER \n");
        }

        Debug::printf("*** AB (2, 1) = %d\n", ab21);
    } else {
        ab22 = (a21 * b12) + (a22 * b22);

        //all other members of answer matrix should already have been modified
        if (ab11 == 0 or ab12 == 0 or ab21 == 0) {
            Debug::printf("*** FAIL: WRONG EXECUTION ORDER \n");
        }

        Debug::printf("*** AB (2, 2) = %d\n", ab22);
    }
    productCalculation++;
}

//computes first diagonal product and then second diagonal product (in that order) of the product matrix
//the core that enters is assigned to the next step in the process above
//global counter variable determinantCalculation is updated by the last core who entered this function to keep process sequential
//steps should be performed in correct order based on incrementing of global counter variable by cores in a critical section
void diagonal(void) {
    //first core who enters calculates first diagonal product
    if (determinantCalculation == 0) {
        d1 = ab11 * ab22;
    }

    //second core who enters calculates second diagonal product
    else if (determinantCalculation == 1) {
        d2 = ab12 * ab21;
    }

    determinantCalculation++;
}

void calculateDeterminant(void) {
    //this tests that another critical section can be entered from the core who is already in one
    critical(diagonal);
}

void kernelMain(void) {
    //first, we calculate the product of the two matrices
    critical(computeNextResult);

    //sync up our cores so we have the complete product before taking its determinant
    barrier1->sync();

    //next, we calculate the determinant of the resultant matrix
    critical(calculateDeterminant);

    //sync up our cores so we have both pieces of calculation to determine determinant
    barrier2->sync();

    //prints the determinant calculated by one core so output is not duplicated
    if (SMP::me() == 3) {
        Debug::printf("*** DET(AB) = %d\n", d1 - d2);
    }
    
}