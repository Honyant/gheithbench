#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"
#include "threads.h"
#include "debug.h"
#include "barrier.h"

const uint32_t THREADS = 25;

uint32_t checksums_first_half[THREADS];
uint32_t checksums_second_half[THREADS];
uint32_t first_half_checksum_idx = 0;
uint32_t second_half_checksum_idx = 0;
SpinLock lock;

/*
I thought this was pretty cool we can undefine the ASSERT statement and redefine it to our own just for this .cc file
so it won't affect any ASSERT statements written in your code.
*/
#undef ASSERT
#define ASSERT(x) if (!(x)) { \
    Debug::printf("*** Test Failed -- sanity check your find()\n"); \
    Debug::shutdown(); \
}

void show(const char* name, StrongPtr<Node> node, bool show) {

    Debug::printf("*** looking at %s\n",name);

    if (node == nullptr) {
        Debug::printf("***      does not exist\n");
        return;
    } 

    if (node->is_dir()) {
        Debug::printf("***      is a directory\n");
        Debug::printf("***      contains %d entries\n",node->entry_count());
        Debug::printf("***      has %d links\n",node->n_links());
    } else if (node->is_symlink()) {
        Debug::printf("***      is a symbolic link\n");
        auto sz = node->size_in_bytes();
        Debug::printf("***      link size is %d\n",sz);
        auto buffer = new char[sz+1];
        buffer[sz] = 0;
        node->get_symbol(buffer);
        Debug::printf("***       => %s\n",buffer);
    } else if (node->is_file()) {
        Debug::printf("***      is a file\n");
        auto sz = node->size_in_bytes();
        Debug::printf("***      contains %d bytes\n",sz);
        Debug::printf("***      has %d links\n",node->n_links());
        if (show) {
            auto buffer = new char[sz+1];
            buffer[sz] = 0;
            auto cnt = node->read_all(0,sz,buffer);
            CHECK(sz == cnt);
            CHECK(K::strlen(buffer) == cnt);
            // can't just print the string because there is a 1000 character limit
            // on the output string length.
            Debug::printf("*** ");
            for (uint32_t i=0; i<cnt; i++) {
                Debug::printf("%c",buffer[i]);
            }
            delete[] buffer;
            Debug::printf("\n");
        }
    } else {
        Debug::printf("***    is of type %d\n",node->get_type());
    }
}

uint32_t computeCheckSum(char* buffer, uint32_t n) {
    uint32_t sum = 0;
    for (uint32_t i=0; i<n; i++) {
        sum += buffer[i];
    }
    return sum;
}

void readAndValidate(StrongPtr<Node> node, uint32_t offset, uint32_t n, bool is_first_half) {
    char* buffer = new char[n];
    node->read_all(offset,n,buffer);

    uint32_t checksum = computeCheckSum(buffer,n);
    lock.lock();
    if (is_first_half) {
        checksums_first_half[first_half_checksum_idx++] = checksum;
    } else {
        checksums_second_half[second_half_checksum_idx++] = checksum;
    }
    lock.unlock();
    delete[] buffer;
}

/* Called by one core */
void kernelMain(void) {

    auto ide = StrongPtr<Ide>::make(1, 3);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);
    
    auto root = fs->root;
    
    {
        Debug::printf("*** Sanity Checking find()\n");
        auto root2 = fs->find(root,"circular/symlink_to_root/");
        Debug::printf("*** root number is %d = %d\n",root->number,root->number);
        ASSERT(root2->number == root->number);

        auto nested = fs->find(root, "circular/nested");
        auto nested2 = fs->find(root, "circular/nested/symlink_to_nested/");
        Debug::printf("*** nested number is %d = %d\n",nested->number,nested2->number);
        ASSERT(nested2->number == nested->number);

        auto circular = fs->find(nested, "../");
        auto circular2 = fs->find(nested, "../../circular/nested/deeper/symlink_back_to_circular/");
        Debug::printf("*** circular number is %d = %d\n",circular->number,circular2->number);
        ASSERT(circular2->number == circular->number);

        auto file_txt = fs->find(circular, "file.txt");
        show("/circular/file.txt",file_txt,true);

        auto deep_file_txt = fs->find(circular, "nested/deeper/symlink_back_to_circular/nested/symlink_to_nested/deeper/deep_file.txt");
        show("/circular/nested/deeper/deep_file.txt",deep_file_txt,true);
    }

    /*
        Now on to the fun part let's test the cache!

        We create a checksum (numerical representation for easy comparison) for the block read from the file
        and validate that the checksums are the same for all threads reading the same block. Hopefully this 
        causes hash conflicts in the cache and will cause incorrect checksums to be calculated if there is 
        no proper synchronization.

    */ 

    auto large_file_txt = fs->find(root, "large_random.txt");
    uint32_t file_size = large_file_txt->size_in_bytes();
    uint32_t half_size = file_size / 2;

    for (uint32_t i=0; i<THREADS; i++) {
        thread([large_file_txt, half_size] {
            readAndValidate(large_file_txt, 0, half_size, true);
        });

        thread([large_file_txt, half_size] {
            readAndValidate(large_file_txt, half_size, half_size, false);
        });
    }

    while (first_half_checksum_idx < THREADS || second_half_checksum_idx < THREADS) {
        yield();
    }

    Debug::printf("*** Validating checksums...\n");
    for (uint32_t i = 1; i < first_half_checksum_idx; ++i) {
        if (checksums_first_half[i] != checksums_first_half[0]) {
            Debug::printf("*** Mismatch in checksums for the first half: %u vs %u\n",
                checksums_first_half[i], checksums_first_half[0]);
        }
    }
    for (uint32_t i = 1; i < second_half_checksum_idx; ++i) {
        if (checksums_second_half[i] != checksums_second_half[0]) {
            Debug::printf("*** Mismatch in checksums for the second half: %u vs %u\n",
                checksums_second_half[i], checksums_second_half[0]);
        }
    }
    Debug::printf("*** Checksums validated\n");

}

