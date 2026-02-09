#include "debug.h"
#include "critical.h"
#include "atomic.h"

static int counter = 0;

void drawHi() {
    /*

    This function will draw the word hi over 4 function calls. 
    Each call will be responsible for drawing a portion of the word, as shown below:

    ***   ***  ***   <- counter = 0
    ***   ***  ***  
    ***   ***        <- counter = 1  
    *********  ***
    *********  ***   <- counter = 2
    ***   ***  ***
    ***   ***  ***   <- counter = 3
    ***   ***  ***

    This will only draw correctly if run sequentially, allowing counter to update after each call.

    */
    switch (counter) {
    case 0:
    case 4:
        Debug::printf("***   ***  ***\n");
        Debug::printf("***   ***  ***\n");
        break;
    case 1:
    case 5:
        Debug::printf("***   ***     \n");
        Debug::printf("*********  ***\n");
        break;
    case 2:
    case 6:
        Debug::printf("*********  ***\n");
        Debug::printf("***   ***  ***\n");
        break;
    case 3:
    case 7:
        Debug::printf("***   ***  ***\n");
        Debug::printf("***   ***  ***\n");
        Debug::printf("\n");
        break;
    }
    counter++;
}


/* Called by all cores */
void kernelMain(void) {
    critical(drawHi);
    
    // Wait for the hi drawing to complete (all cores have run drawHi)
    while (counter != 4);

    // draw it again! just to catch any bugs that may exist with a second critical section
    // note this time the counter will start at 4
    critical(drawHi);

}
