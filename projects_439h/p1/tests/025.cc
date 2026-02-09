#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter = 0;

void work()
{
    if (counter == 2) {
        counter = 0;
        return;
    }
    if (counter % 2 == 0)
    {
        Debug::printf("*** I like tacos\n");
    }
    else
    {
        Debug::printf("*** Do you like tacos?\n");
    }
    counter++;
    critical(work);
    Debug::printf("*** TACOS!\n");
}

/* Called by all cores */
void kernelMain(void)
{
    // The C way, use function pointers
    critical(work);
}
