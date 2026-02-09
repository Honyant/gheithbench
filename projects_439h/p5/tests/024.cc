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
    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 3);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);
    
    auto root = fs -> root;

   // Test 1: Basic Tests

   auto flavor = fs -> find(root, "f1/flavor");
   show("/f1/flavor", flavor, true);
   auto size = flavor -> size_in_bytes();
   auto buffer1 = new char[size];
   flavor -> read_all(0, size, buffer1);
   Debug::printf("*** %s\n", buffer1);

   auto sym_to_f1 = fs -> find(root, "root_to_f1");
   show("/root_to_f1", sym_to_f1, true);

   auto flavor_sym = fs -> find(root, "root_to_f1/flavor");
   CHECK(flavor_sym -> number == flavor -> number);
   show("/root_to_f1/flavor", flavor_sym, true);
   size = flavor_sym -> size_in_bytes();
   auto buffer2 = new char[size];
   flavor_sym -> read_all(0, size, buffer2);
   Debug::printf("*** %s\n", buffer2);
   Debug::printf("*** Basic Tests Passed!\n");

   // Test 2: Circular SymLinks
   auto f1 = fs -> find(root, "f1");
   auto f1_to_root_to_f1 = fs -> find(f1, "f1_to_root/root_to_f1/");
   CHECK(f1_to_root_to_f1 -> number ==  f1 -> number);

   auto two_level = fs -> find(root, "root_to_f1/f1_to_root/root_to_f1/f1_to_root/");
   CHECK(two_level -> number == root -> number);

   auto three_level = fs -> find(f1, "f1_to_root/root_to_f1/f1_to_root/root_to_f1/f1_to_root/root_to_f1/");
   CHECK(three_level -> number == f1 -> number);


   // Test 3: Reading a big file
   auto bee = fs -> find(root, "bee.txt");
   show("/bee.txt", bee, false);
   auto start = 66450;
   auto end = 66525;

   auto bee_buffer = new char[end - start + 1];
   bee -> read_all(start, end - start, bee_buffer);
   Debug::printf("*** %s\n", bee_buffer);

   auto temp1 = fs -> root;
   auto temp2 = fs -> root;
   auto temp3  = fs -> root;
    // Test 4: Simple Caching Test
    for (int i = 0; i < 100; i++) {
        temp1 = fs -> find(root, "f1/flavor");
        temp2 = fs -> find(root, "root_to_f1/flavor");
        temp3 = fs -> find(root, "root_to_f1/f1_to_root/root_to_f1/flavor");
    }
   
}

