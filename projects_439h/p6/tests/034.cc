#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"

// Simple test that mmap's virtual addresses are consistently the minimum possible

void kernelMain(void) {

    using namespace VMM;


    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 1);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);
   
    // get "/"
    auto root = fs->root;

    //  get "/hello"
    auto hello = fs->find(root,"hello");

    for (char* expected_address = (char*) 0x80000000; expected_address < (char*) 0x80100000; expected_address += 0x00001000) {
        auto p = (char*) naive_mmap(hello->size_in_bytes(),false,hello,0);
        CHECK(p == expected_address);
    }
}
