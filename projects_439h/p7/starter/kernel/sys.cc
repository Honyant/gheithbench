#include <stdint.h>

#include "sys.h"
#include "stdint.h"
#include "debug.h"
#include "idt.h"
#include "machine.h"

extern "C" int sysHandler(uint32_t eax, uint32_t *frame) {
    //Debug::printf("*** syscall #%d\n",eax);
    switch (eax) {
    case 7: /* shutdown */
        Debug::shutdown();
        return -1;

    default:
        Debug::printf("*** unknown system call %d\n",eax);
        return -1;
    }
    
}   

void SYS::init(void) {
    IDT::trap(48,(uint32_t)sysHandler_,3);
}
