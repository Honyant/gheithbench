#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter = 0;

//this test checks whether you have implemented depth handling by calling citical inside critical recursively
//It also calls three criticals righ after each other to attempt triggering race conditions
//printing out things in the right order is important as this generates a story

void work()
{
    //last few prints should be ignored, checks if counter is updated properly
    if (counter < 10) {
        if (counter == 0) {
            Debug::printf("*** h2o: Just Add Water!! S5 EP1: The Moon Rises with the NAUR spell \n");
        }

        //Emma's lines!
        if (counter % 3 == 0 && counter < 3) {
            Debug::printf("*** Emma: Guys, the full moon is tonight \n");
        } else if (counter % 3 == 0 && counter < 6) {
            Debug::printf("*** Emma: We should go to my house and protect ourselves from the moon spell \n");
        } else if (counter % 3 == 0 && counter < 8) {
            Debug::printf("*** Emma: Cleo may already be under it unfortunately... \n");
        } else if (counter % 3 == 0) {
            Debug::printf("*** THE END!! Go watch h2o- it's a great show! \n");
        }

        //naurrrr!!!
        if (counter % 3 == 2 && counter < 6) {
            Debug::printf("*** Cleo: NAURRRRRR \n");
        } else if (counter % 3 == 2) {
            Debug::printf("*** Cleo: naurrrrrrrrrrrrrrr... \n");
        }

        //Rikkiiiii!!!!
        if (counter % 3 == 1 && counter < 3) {
            Debug::printf("*** Rikki: It came up on us so quickly!! D: \n");
        } else if (counter % 3 == 1 && counter < 6) {
            Debug::printf("*** Rikki: Cleo, can you stop saying naur? \n");
        } else if (counter % 3 == 1) {
            Debug::printf("*** Rikki: Why does this always happen to her... \n");
        }

        //Is counter updated properly keeping in mind race conditions?
        counter++;

    }
    
}

/* Called by all cores */
void kernelMain(void)
{
    //recursive critical call- depth test 1
    critical([] {work(); critical(work);});

    //criticals called right after each other- depth test 2
    critical(work);
    critical(work); //this one shouldn't be doing anything really if the counter is updated properly
}
