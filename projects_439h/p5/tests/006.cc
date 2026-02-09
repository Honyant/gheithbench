#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"
#include "threads.h"

#define LINE_LENGTH 99
#define THREAD_COUNT 50
// Just a nice little multithreaded reading test :)
uint32_t counter = 0;
void count_up(StrongPtr<Ext2> fs, uint32_t i) {
    auto root = fs->root;

    auto offset = i * LINE_LENGTH;
    
    
    auto buffer = new char[LINE_LENGTH + 1]{0};
    if (i % 2){
        auto node = fs->find(root, "gheithed");
        node->read_all(offset, LINE_LENGTH, buffer);
    } else {
        auto node = fs->find(root, "/gheithed"); //Let's make sure you can read from root too
        node->read_all(offset, LINE_LENGTH, buffer);
    }
    while (counter != i) 
        yield();
    Debug::printf("%s", buffer);
    __atomic_add_fetch(&counter, 1, __ATOMIC_SEQ_CST);
    
    if (i + THREAD_COUNT < 100)
        thread([fs, i]() mutable { count_up(fs, i + THREAD_COUNT); });
    
}

void kernelMain(void) {
    auto ide = StrongPtr<Ide>::make(1, 3);
    auto fs = StrongPtr<Ext2>::make(ide);

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
        thread(
            [fs, i]() mutable {
                count_up(fs, i);
            }
        );
    }
    while (counter != 100)
        yield();

}

