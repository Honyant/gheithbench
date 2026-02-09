#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

/* Test if you have a cache that stores at least the last 3 blocks read */
void kernelMain(void) {
    uint32_t delay_ms = 300;
    auto ide = StrongPtr<Ide>::make(1, delay_ms);
    auto fs = StrongPtr<Ext2>::make(ide);

    auto a_file = fs->find(fs->root,"a.txt");
    uint32_t character_indices[3];

    // Within the 12 direct blocks
    character_indices[0] = 50;

    // Within the 1-indirect block
    character_indices[1] = 15000;

    // Within the 2-indirect block
    character_indices[2] = 290000;
    
    for (uint32_t i = 0; i < 50; i++) {
        char c;
        a_file->read(character_indices[i % 3], c);
        Debug::printf("*** %c\n", c);
    }
}

