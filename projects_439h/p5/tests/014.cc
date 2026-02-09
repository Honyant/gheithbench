#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

void show(const char* name, StrongPtr<Node> node, bool show) {

    Debug::printf("*** looking at %s\n",name);

    if (node == nullptr) {
        Debug::printf("***      does not exist\n");
        return;
    }

    Debug::printf("---      inode #%d\n",node->number);

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
    auto ide = StrongPtr<Ide>::make(1, 3);
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());
   
    auto root = fs->root;
    show("/",root,true);

    // Big file requiring indirect blocks
    auto chonky = fs->find(root,"chonky.txt");
    show("/chonky.txt", chonky, false);

    // Read the 27th block of the file
    Debug::printf("***      end: ");
    char* buffer = new char[1024];
    uint32_t m = chonky->read_all(27 * 1024, 1024, buffer);
    for (uint32_t i = 0; i < m; i++) {
       Debug::printf("%c", buffer[i]);
    }

    // Long symlink names around 60 chars
    auto symlink1 = fs->find(root, "symlink1");
    show("/symlink1", symlink1, true);

    auto symlink2 = fs->find(root, "symlink2");
    show("/symlink2", symlink2, true);

    auto symlink3 = fs->find(root, "symlink3");
    show("/symlink3", symlink3, true);

    // Big directory requiring indirect nodes
    auto spam = fs->find(root, "spam");
    show("/spam", spam, true);
    Debug::printf("***      takes up %d bytes\n", spam->size_in_bytes());

    // Make sure you can read all the entries by checking a subset of them
    Debug::printf("*** Checking files in spam folder\n");
    for (char c1 = 'a'; c1 <= 'z'; c1 += 2) {
        for (char c2 = 'a'; c2 <= 'z'; c2 += 2) {
            char name[] = {c1, c2, 0};
            auto node = fs->find(spam, name);
            if (!(node->is_file() && node->size_in_bytes() == 0)) {
                Debug::printf("*** Problem in file %s:\n", name);
                show(name, node, true);
            }
        }
    }

    // Test directory finding stuff (including / and symlinks)
    auto linkies = fs->find(root, "linkies");
    show("/linkies", linkies, true);
    show("/linkies/root", fs->find(linkies, "root"), true);

    auto root2 = fs->find(linkies, "/");
    auto root3 = fs->find(linkies, "root/");
    auto root4 = fs->find(linkies, "root/linkies/root/");
    auto root5 = fs->find(linkies, "/./linkies/../linkies/root/linkies/root/");

    if (root->number != root2->number) {
        Debug::printf("*** root (#%d) != root2 (#%d)\n", root->number, root2->number);
        show("/", root2, true);
    }
    if (root->number != root3->number) {
        Debug::printf("*** root (#%d) != root3 (#%d)\n", root->number, root3->number);
        show("root/", root3, true);
    }
    if (root->number != root4->number) {
        Debug::printf("*** root (#%d) != root4 (#%d)\n", root->number, root4->number);
        show("root/linkies/root/", root4, true);
    }
    if (root->number != root5->number) {
        Debug::printf("*** root (#%d) != root5 (#%d)\n", root->number, root5->number);
        show("/./linkies/../linkies/root/linkies/root/", root5, true);
    }

    Debug::printf("*** Done! :)\n");
}

