#include "debug.h"
#include "ide.h"
#include "ext2.h"
#include "elf.h"
#include "machine.h"
#include "libk.h"
#include "config.h"
#include "shared.h"
#include "vmm.h"
#include "threads.h"
#include "u8250.h"

void initFDT(){}

void kernelMain(void) {
    auto d = StrongPtr<Ide>::make(1,0);
    auto fs = StrongPtr<Ext2>::make(d);
    auto init = fs->find(fs->root, "/sbin/init");
    auto me = impl::threads::state.current();
    me->cwd = fs->root;

    me->id = 1;
    me->file_descriptors[0].id = 0;
    me->file_descriptors[0].file = new OpenFile();
    me->file_descriptors[0].file->isStdin = true;
    me->file_descriptors[0].file->canRead = true;
    me->file_descriptors[1].id = 1;
    me->file_descriptors[1].file = new OpenFile();
    me->file_descriptors[1].file->isStdout = true;
    me->file_descriptors[1].file->canWrite = true;
    me->file_descriptors[2].id = 2;
    me->file_descriptors[2].file = new OpenFile();
    me->file_descriptors[2].file->isStderr = true;
    me->file_descriptors[2].file->canWrite = true;


    Debug::printf("loading init\n");
    uint32_t e = ELF::load(init);
    Debug::printf("entry %x\n",e);
    uint32_t userEsp = 0xF0000000 - 16; // where did this come from
    VMM::mmap(1 << 27, false, StrongPtr<Node> {},0b110, 0, (void *) (0xF0000000 - (1 << 27)));

    uint32_t argc = 1;
    char *argv[] = { (char *)"init", nullptr };
    uint32_t arg_addresses[argc];

    // copy to stack and record addy
    for (int i = argc - 1; i >= 0; i--) {
        char *arg = argv[i];
        int len = K::strlen(arg) + 1;
        userEsp -= len;
        memcpy((void *)userEsp, arg, len);
        arg_addresses[i] = userEsp;
    }
    // argv pointers
    userEsp = (userEsp & ~0x3) - 4;
    *((uint32_t *)userEsp) = 0;
    for (int i = argc - 1; i >= 0; i--)
    *((uint32_t *)(userEsp -= 4)) = arg_addresses[i];
    // argv and argc
    userEsp -= 4;
    *((uint32_t *)userEsp) = userEsp + 4;
    *((uint32_t *)(userEsp -= 4)) = argc;


    // for(uint32_t i = userEsp; i < 0xF0000000 - 4090; i += 4) {
    //     Debug::printf("addr %x: %x --- ", i,  *(uint32_t*)i);
    //     for(uint32_t j = 0; j < 4; j++) {
    //         Debug::printf("%c",*(uint8_t*)(i+j));
    //     }
    //     Debug::printf("\n");
    // }

    // Debug::printf("a %x, end %x\n", a, (uint32_t) a + 10 * 4096);
    // Debug::printf("user esp %x\n",userEsp);
    
    // Current state:
    //     - %eip points somewhere in the middle of kernelMain
    //     - %cs contains kernelCS (CPL = 0)
    //     - %esp points in the middle of the thread's kernel stack
    //     - %ss contains kernelSS
    //     - %eflags has IF=1
    // Desired state:
    //     - %eip points at e
    //     - %cs contains userCS (CPL = 3)
    //     - %eflags continues to have IF=1
    //     - %esp points to the bottom of the user stack
    //     - %ss contain userSS
    // User mode will never "return" from switchToUser. It will
    // enter the kernel through interrupts, exceptions, and system
    // calls.
    switchToUser(e,userEsp,0);
}

