#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "barrier.h"

template <typename T, typename Work>
inline StrongPtr<Promise<T>> future(Work const& work) {
    auto p = StrongPtr<Promise<T>>::make();
    thread([p, work] {
        p->set(work());
    });
    return p;
}

/* Called by one CPU */
void kernelMain(void) {

    using namespace VMM;


    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 1);

    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);

    // get "/"
    auto root = fs->root;

    //  get "/hi.txt"
    auto hi = fs->find(root,"hi.txt");

    CHECK(hi != nullptr);

    char* hi_p = reinterpret_cast<char*>(naive_mmap(hi->size_in_bytes(), false, hi, 0));

    CHECK(hi_p == (char*) 0x80000000);

    uint32_t line_size = 32; //26 letters + 2 spaces + newline + ***
    uint32_t num_lines = 6;

    for (uint32_t line = 0; line < num_lines; line++) {
        for(uint32_t car = 0; car < line_size; car++) {
            Debug::printf("%c", hi_p[line * line_size + car]);
        }
    }
    
    //mmap starting at an offset
    char* hi_offset = reinterpret_cast<char*>(naive_mmap(hi->size_in_bytes()-line_size, false, hi, line_size));
    CHECK(hi_offset == (char*) 0x80001000);

    num_lines = 5;

    //shouldn't print the first line (lowercase alphabet)
    for (uint32_t line = 0; line < num_lines; line++) {
        for(uint32_t car = 0; car < line_size; car++) {
            Debug::printf("%c", hi_offset[line * line_size + car]);
        }
    }

    auto spam = fs->find(root, "a_directory/spam.txt");

    CHECK(spam != nullptr);

    naive_munmap(hi_p);

    char* spam_p = reinterpret_cast<char*>(naive_mmap(spam->size_in_bytes(), false, spam, 0));

    Debug::printf("spam_p %p\n", spam_p);

    //can't be at unmapped location of 0x80000000; it's not large enough
    //if you fail this check your free page logic
    CHECK(spam_p == (char*) 0x80002000);
    CHECK(spam_p[4095] == '\n'); //page fault at last address in page 1
    CHECK(spam_p[8191] == '\n'); //page fault at last address in page 2
    CHECK(spam_p[4094] == 'a'); //access second to last address in page 1
    CHECK(spam_p[8190] == 'b'); //access second to last address in page 2
    CHECK(spam_p[0] == 'a'); //access first address in page 1
    CHECK(spam_p[4096] == 'b'); //acces first address in page 2

    auto spam2 = fs->find(root, "a_directory/spam2.txt");

    naive_munmap(hi_offset);
    //now there's two pages available starting at 0x80000000
    //100 is a magic number here
    //number of extra characters in spam2 vs spam
    //99 ! and a newline
    char* spam_offset = reinterpret_cast<char*>(naive_mmap(spam2->size_in_bytes()-100, false, spam2, 100));
    CHECK(spam_offset == (char*) 0x80000000);
    //repeat the checks from spam
    //make sure page fault reads offsets correctly
    CHECK(spam_offset[4095] == '\n'); 
    CHECK(spam_offset[8191] == '\n'); 
    CHECK(spam_offset[4094] == 'a');
    CHECK(spam_offset[8190] == 'b');
    CHECK(spam_offset[0] == 'a'); 
    CHECK(spam_offset[4096] == 'b');

    //check threads have their own private space
    int num_threads = 10;
    StrongPtr<Barrier> b = StrongPtr<Barrier>::make(num_threads+1);

    for (int i = 0; i < 10; i++) {
        thread([b, hi] {
            auto p = (char*) naive_mmap(hi->size_in_bytes(), false, hi, 0);
            CHECK(p == (char*) 0x80000000);
            b->sync();
        });
    }
    b->sync();

    auto hi_shared = (char*) naive_mmap(hi->size_in_bytes(), true, hi, 0);
    CHECK(hi_shared == (char*) 0xF0000000);

    hi_shared[0] = -1;

    StrongPtr<Barrier> b2 = StrongPtr<Barrier>::make(num_threads+1);
    for (int i = 0; i < 10; i++) {
        thread([b2, hi_shared, i] {
            CHECK(hi_shared[0] != 'a');
            hi_shared[0] = i;
            b2->sync();
        });
    }
    b2->sync();
}