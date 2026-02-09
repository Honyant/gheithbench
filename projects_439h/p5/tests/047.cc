#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

// void show(const char* name, StrongPtr<Node> node, bool show) {

//     Debug::printf("*** looking at %s\n",name);

//     if (node == nullptr) {
//         Debug::printf("***      does not exist\n");
//         return;
//     } 

//     if (node->is_dir()) {
//         Debug::printf("***      is a directory\n");
//         Debug::printf("***      contains %d entries\n",node->entry_count());
//         Debug::printf("***      has %d links\n",node->n_links());
//     } else if (node->is_symlink()) {
//         Debug::printf("***      is a symbolic link\n");
//         auto sz = node->size_in_bytes();
//         Debug::printf("***      link size is %d\n",sz);
//         auto buffer = new char[sz+1];
//         buffer[sz] = 0;
//         node->get_symbol(buffer);
//         Debug::printf("***       => %s\n",buffer);
//     } else if (node->is_file()) {
//         Debug::printf("***      is a file\n");
//         auto sz = node->size_in_bytes();
//         Debug::printf("***      contains %d bytes\n",sz);
//         Debug::printf("***      has %d links\n",node->n_links());
//         if (show) {
//             auto buffer = new char[sz+1];
//             buffer[sz] = 0;
//             auto cnt = node->read_all(0,sz,buffer);
//             CHECK(sz == cnt);
//             CHECK(K::strlen(buffer) == cnt);
//             // can't just print the string because there is a 1000 character limit
//             // on the output string length.
//             for (uint32_t i=0; i<cnt; i++) {
//                 Debug::printf("%c",buffer[i]);
//             }
//             delete[] buffer;
//             Debug::printf("\n");
//         }
//     } else {
//         Debug::printf("***    is of type %d\n",node->get_type());
//     }
// }

void checkDir(StrongPtr<Node> node) {
    ASSERT(node->is_dir());
}

void checkFile(StrongPtr<Node> node) {
    ASSERT(node->is_file());
}

void showFile(StrongPtr<Node> node) {
    checkFile(node);

    auto sz = node->size_in_bytes();
    auto buffer = new char[sz+1];
    buffer[sz] = 0;
    auto cnt = node->read_all(0,sz,buffer);
    ASSERT(sz == cnt);
    ASSERT(K::strlen(buffer) == cnt);
    // can't just print the string because there is a 1000 character limit
    // on the output string length.
    for (uint32_t i=0; i<cnt; i++) {
        Debug::printf("%c",buffer[i]);
    }
    delete[] buffer;
}

void kernelMain(void) {
    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 3);

    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n", fs->get_block_size());
    Debug::printf("*** inode size is %d\n", fs->get_inode_size());

    // this test case tests functionality in chained directories

    // get "/"
    auto root = fs->root;
   
    // get "/onefile"
    auto onefile = fs->find(root, "onefile");
    
    // get "/one"
    auto one = fs->find(root, "one");
    
    // get "/one/twofile"
    auto twofile = fs->find(one, "twofile");
    
    // get "/one/two"
    auto two = fs->find(one, "two");
    
    // get "/one/two/threefile"
    auto threefile = fs->find(two, "threefile");
    
    // get "/one/two/three"
    auto three = fs->find(two, "three");
    
    // get "/one/two/three/fourfile"
    auto fourfile = fs->find(three, "fourfile");
    
    // get "/one/two/three/four"
    auto four = fs->find(three, "four");
    
    // get "/one/two/three/four/fivefile"
    auto fivefile = fs->find(four, "fivefile");
    
    // get "/one/two/three/four/five"
    auto five = fs->find(four, "five");
    
    // get "/one/two/three/four/five/six"
    auto six = fs->find(five, "six");
    
    // get "/one/two/three/four/five/six/seven"
    auto seven = fs->find(six, "seven");
    
    // get "/one/two/three/four/five/six/seven/eight"
    auto eight = fs->find(seven, "eight");
    
    // get "/one/two/three/four/five/six/seven/eight/nine"
    auto nine = fs->find(eight, "nine");
    
    // get "/one/two/three/four/five/six/seven/eight/nine/ten"
    auto ten = fs->find(nine, "ten");
    
    // get "/one/two/three/four/five/six/seven/eight/nine/ten/youmadeit"
    auto youmadeit = fs->find(ten, "youmadeit");


    checkDir(root);
    checkDir(one);
    checkDir(two);
    checkDir(three);
    checkDir(four);
    checkDir(five);
    checkDir(six);
    checkDir(seven);
    checkDir(eight);
    checkDir(nine);
    checkDir(ten);

    checkFile(onefile);
    checkFile(twofile);
    checkFile(threefile);
    checkFile(fourfile);
    checkFile(fivefile);
    checkFile(youmadeit);

    showFile(onefile);
    showFile(twofile);
    showFile(threefile);
    showFile(fourfile);
    showFile(fivefile);
    showFile(youmadeit);

    // Check that each directory points to itself via "."
    ASSERT(root->number == fs->find(root, ".")->number);
    ASSERT(one->number == fs->find(one, ".")->number);
    ASSERT(two->number == fs->find(two, ".")->number);
    ASSERT(three->number == fs->find(three, ".")->number);
    ASSERT(four->number == fs->find(four, ".")->number);
    ASSERT(five->number == fs->find(five, ".")->number);
    ASSERT(six->number == fs->find(six, ".")->number);
    ASSERT(seven->number == fs->find(seven, ".")->number);
    ASSERT(eight->number == fs->find(eight, ".")->number);
    ASSERT(nine->number == fs->find(nine, ".")->number);
    ASSERT(ten->number == fs->find(ten, ".")->number);

    // Check that each directory points to the correct parent directory via ".."
    ASSERT(root->number == fs->find(one, "..")->number);
    ASSERT(one->number == fs->find(two, "..")->number);
    ASSERT(two->number == fs->find(three, "..")->number);
    ASSERT(three->number == fs->find(four, "..")->number);
    ASSERT(four->number == fs->find(five, "..")->number);
    ASSERT(five->number == fs->find(six, "..")->number);
    ASSERT(six->number == fs->find(seven, "..")->number);
    ASSERT(seven->number == fs->find(eight, "..")->number);
    ASSERT(eight->number == fs->find(nine, "..")->number);
    ASSERT(nine->number == fs->find(ten, "..")->number);
}
