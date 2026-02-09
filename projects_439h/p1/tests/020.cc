#include "debug.h"
#include "critical.h"
#include "atomic.h"
#include "random.h"

static Barrier barrier1(kConfig.totalProcs);
static Barrier barrier2(kConfig.totalProcs);

//simple function to test nested recursion
static uint32_t counter = 0;
void nestedRecursion() {
    Debug::printf("*** func call %d\n",counter);
    counter++;
}

//use cores to sum up diff parts of an array
const int ARRLEN = 10000;
const int SUBLEN = ARRLEN/4;
int globalArray[ARRLEN];
static int globalSum = 0;
static Atomic<int> iter{0};
void sumArrayPart(int start, int end) {
    int localSum = 0;
    for (int i = start; i < end; i++) {
        localSum += globalArray[i];
    }
    critical([localSum]() {
        globalSum += localSum;
    });
}

static uint32_t coreNum = 0;
void kernelMain(void) {
    //nested recursive calls to critical
    critical([]{
        Debug::printf("*** entering recursion depth 0\n");
        nestedRecursion();
        critical([]{
            Debug::printf("*** entering recursion depth 1\n");
            nestedRecursion();
            critical([]{
                Debug::printf("*** entering recursion depth 2\n");
                nestedRecursion();
                Debug::printf("*** exiting recursion depth 2\n");
            });
            Debug::printf("*** exiting recursion depth 1\n");
        });
        Debug::printf("*** exiting recursion depth 0\n");
        Debug::printf("*** core %d end\n", coreNum);
        coreNum++;
    });

    //make array of 1 to 10000
    if (SMP::me() == 0) {
        for (int i = 0; i < ARRLEN; i++) {
            globalArray[i] = i + 1;
        }
    }
    barrier1.sync();

    //call sumarraypart, each core is responsible for adding a diff part
    critical([]{
        uint32_t core = SMP::me();
        int start = core * SUBLEN;
        int end = (core == 3) ? ARRLEN : start + SUBLEN;
        sumArrayPart(start, end);
    });
    barrier2.sync();
    
    //print total sum
    if (SMP::me() == 0) {
        critical([]{
            Debug::printf("*** array sum = %d\n", globalSum);
        });
    }
}
