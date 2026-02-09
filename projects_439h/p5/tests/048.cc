#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"
#include "bb.h"

// tc for bb and caching

void show(const char* name, StrongPtr<Node> node, bool show) {

    Debug::printf("*** looking at %s\n",name);

    if (node == nullptr) {
        Debug::printf("***      does not exist\n");
        return;
    } 

    if (node->is_file()) {
        if (show) {
            auto sz = node->size_in_bytes();
            auto buffer = new char[sz+1];
            buffer[sz] = 0;
            auto cnt = node->read_all(0,sz,buffer);
            CHECK(sz == cnt);
            CHECK(K::strlen(buffer) == cnt);
            // can't just print the string because there is a 1000 character limit
            // on the output string length.
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

/* Called by one core */
void kernelMain(void) {

    auto ide = StrongPtr<Ide>::make(1, 10);
     
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());
   
    const char* filename[] = {"ShinichiKudo.txt", "ConanEdogawa.txt", "RanMouri.txt", "AiHaibara.txt", "EriKisaki.txt",
                                "MiwakoSato.txt", "HeijiHattori.txt", "KazuhaToyama.txt", "SonokoSuzuki.txt", "MakotoKyogoku"};
    auto root = fs->root;
    auto bb = new BB<int>(1);

    // extra for loop for stress test 
    for (int j = 0; j < 25; j++) {
        thread([bb]{
            for (int i = 0; i < 10; i++) {
                bb->put(i);
            }
        });
        
        int last = bb->get();
        ASSERT(last == 0);
        auto expected = 0;

        while(last < 9) {
            if (last != expected) {
                Debug::panic("*** expected:%d last:%d\n", expected, last);
            } else {
                auto node = fs->find(root, filename[last]);
                show(filename[last], node, true);
            }
            last = bb->get();
            expected++;
        }
    }
}

