#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter = 0;

void printStars(uint32_t n)
{
    // Base case: Stop recursion after printing 10 lines
    if (n > 10)
    {
        return;
    }

    // Print n stars
    for (uint32_t i = 0; i < n; i++)
    {
        Debug::printf("*");
    }
    Debug::printf("\n");

    // Recursive call to print the next line with one more star
    printStars(n + 1);
}

void work()
{
    Debug::printf("*** Pyramid of stars %d:\n", counter);
    counter++;
    printStars(3); // Start the recursion with 3 stars
}

/* Called by all cores */
void kernelMain(void)
{
    critical(work); // Start the recursive work function
}
