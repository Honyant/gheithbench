#include "debug.h"
#include "critical.h"
#include "atomic.h"


// This test checks a variety of possible vulnerabilities for recursionless critical sections, as well as
// verifying that basic multi-level critical sections (calling critical inside a call to critical) works
static uint32_t counter = 2;
static bool finalValPrinted = false;
static uint32_t multiLevelCounter = 1;

static Barrier* doneWithPt1 = new Barrier(kConfig.totalProcs);


// This doubles multiLevelCounter and prints it
void multiLevelTestDoubledAndPrint(){
    multiLevelCounter = multiLevelCounter * 2;
    Debug::printf("*** RecursiveValue: %d\n",multiLevelCounter);
}

// This calls another critical function to double multiLevelCounter and print it, verifying that we can run a critical section
// inside of a critical section
void multiLevelTestTopLevel(){
    critical(multiLevelTestDoubledAndPrint);
}

// This function is used to verify that only one core can run the critical section at a time, without any multi level critical calls.
void basicCriticalTest() {
    // Run different code for the final core to verify that the counter is correct at the start of work
    if(counter != 0){

        // Print the current counter
        Debug::printf("*** Initial Value: %d\n",counter);

        // Add and subtract from the counter, to verify that this change will not affect any other cores
        counter += 100;
        Debug::printf("*** Changed Value: %d\n",counter);
        counter -= 100;

        // Print what that the next initial value will be the current value squared plus 1
        // We are using that weird way of incrementing the counter to ensure that the increment is also occurring in sequence rather than in parrallel
        int counterMultiplier = counter;
        Debug::printf("*** Initial Value Again: %d, Next initial value will be that value squared minus one (%d) \n",counter, (counter * counterMultiplier -1));

        counter= counterMultiplier * counter - 1;
    }
    else{
        // Print the final val if we have not already (we get here repeatedly at the end)
        if(!finalValPrinted){

            Debug::printf("*** Final Value: %d. Great Job!\n\n\n",counter);
            finalValPrinted = true;
        }

    }
}

/* Called by all cores */
void kernelMain(void) {
    // Verify that recursion-less critical sections work
    // We do this in a loop to increase the number of calls to critical and thus increase the chance of race conditions
    for(int i = 0; i < 3; i++){
        critical(basicCriticalTest);
    }

    // Barrier to ensure the output of part one is done before the output of part 2
    doneWithPt1->sync();

    // Verify that basic multi level critical calls work
    // We do this in a loop to increase the odds of race conditions
    for(int i = 0; i < 3; i++){
        critical(multiLevelTestTopLevel);
    }
}
