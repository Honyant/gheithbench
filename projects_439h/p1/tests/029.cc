#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter = 0;
static uint32_t core_count = 0;

void work(int count = 0) {
    Debug::printf("*** count %d\n", count);

    Debug::printf("*** hello %d\n",counter);
    Debug::printf("*** goodbye %d\n",counter);
    counter ++;

    if (count > 0) {
        count--;
        work(count);
    }
}

/* Called by all cores */
void kernelMain(void) {
    // The C way, use function pointers

    critical([]{
        Debug::printf("*** core: %d\n", core_count);

        work();
        for (int i = 0; i < 3; i++) {
            Debug::printf("*** loop count: %d\n", i);
            critical([]{
                work(3);
                critical([]{
                    work(3);
                });
            });
            Debug::printf("*** loop count: %d\n\n", i);
        }

        Debug::printf("*** end of core: %d\n\n", core_count);
        core_count++;
    });

}
