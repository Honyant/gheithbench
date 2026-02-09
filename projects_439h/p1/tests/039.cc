#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t depth = 0;
static uint32_t job_num = 0;
static uint32_t goal_num = 0;
static Barrier* syncer[3] = { new Barrier(kConfig.totalProcs),
                                new Barrier(kConfig.totalProcs),
                                new Barrier(kConfig.totalProcs)};

static bool completed[3] = {false, false, false};
static uint32_t observed_order[8];

//critical section inside a critical section inside a critical section
void work1() {
    critical([] {
        depth++;
        Debug::printf("*** Depth: %d\n", depth);
    });
}

void work() {
    critical([] {
        depth++;
        Debug::printf("*** Depth: %d\n", depth);
        work1();
    });
}

void kernelMain(void) {

    //Make sure each core completes a different task and completes it one at a time
    critical([] {
        job_num++;
        goal_num += 5 * job_num - SMP::me();
    });

    //Let all cores take a break and rejoin here
    syncer[0]->sync();

    critical([] {
        if (!completed[1]) {
            completed[1] = true;
            Debug::printf("*** -----------------------------------------------------------------------\n");
            Debug::printf("*** Did we get the group project among the cores done correctly? Let's see\n");
            Debug::printf("*** Expected number from cores: 44, Received number from cores: %d\n", goal_num);
        }
    });

    syncer[1]->sync();

   //Alternate cores in a specific order
   uint32_t core_order[8] = {0, 2, 3, 1, 1, 0, 0, 0};

   for (int i = 0; i < 8; i++) {
        critical([core_order, i] {
            if (SMP::me() == core_order[i]) {
                observed_order[i] = SMP::me();
            }
        });
   }

   syncer[2]->sync();
   
   for (int i = 0; i < 8; i++) {
        if (core_order[i] != observed_order[i]) {
            Debug::printf("*** Looks like your cores executed in the wrong order...\n");
            Debug::printf("Expected core %d at iteration %d, but recieved core %d\n", core_order[i], i, observed_order[i]);
            Debug::shutdown();
        }
   }

   critical([] {
        if (!completed[2]) {
            completed[2] = true;
            Debug::printf("*** -----------------------------------------------------------------------\n");
            Debug::printf("*** Cores executed in correct order\n");
        }
    });


   critical([] {
        //compute depth once only
        if (!completed[0]) {
            depth++;
            completed[0] = true;
            Debug::printf("*** -----------------------------------------------------------------------\n");
            Debug::printf("*** Depth: %d\n", depth);
            work();
        }
    });
}