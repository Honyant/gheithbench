#include "debug.h"
#include "critical.h"
#include "atomic.h"
#include "smp.h"


static int i = 0;


//Testing critical sections inside each other
void work(){
    if(i < 100){
        i++;
        critical(work);
    }else{
        Debug::printf("*** end of work, i is: %d\n", i);
        i = 0;
    }
}

/* Called by all cores */
void kernelMain(void) {
    critical(work);
}
