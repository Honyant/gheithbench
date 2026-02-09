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
    auto ide = StrongPtr<Ide>::make(1, 3);
    
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** SYMLINK TESTS, TESTS MANY EDGE CASES\n");

    Debug::printf("*** TEST 0: basic find, if you past t0, you should pass this\n");
   
   // get "/"
   auto root = fs->root;
   show("/",root,true);

   //  get "/dir1"
   auto dir1 = fs->find(root,"dir1");
   CHECK(root->number != dir1->number);
   show("/dir1",dir1,false);

   // get "/dir1/dir2"
   auto dir2 = fs->find(root,"dir1/dir2");
   CHECK(dir1->number != dir2->number);
   CHECK(root->number != dir2->number);
   show("/dir2",dir2,false);

    Debug::printf("*** ----------------------------------------------------------------------------------\n");

   Debug::printf("*** TEST 1: find + symlink, test both finding and resolving a relative symlink\n");
   auto sym_link_no_resolve = fs->find(root,"dir1/relativeSym");
    CHECK(sym_link_no_resolve->number != dir1->number);
    show("/dir1/relativeSym",sym_link_no_resolve,false);
    auto sym_link_resolve = fs->find(root,"dir1/relativeSym/");
    CHECK(sym_link_resolve->number != dir1->number);
    show("symlink_test/",sym_link_resolve,true);


    Debug::printf("*** ----------------------------------------------------------------------------------\n");
    

    Debug::printf("*** TEST 2: test 1 for absolute symlinks\n");
    auto sym_link_no_resolve_abs = fs->find(root,"dir1/absoluteSym");
    CHECK(sym_link_no_resolve_abs->number != dir1->number);
    show("/dir1/absoluteSym",sym_link_no_resolve_abs,false);
    auto sym_link_resolve_abs = fs->find(root,"dir1/absoluteSym/");
    CHECK(sym_link_resolve_abs->number != dir1->number);
    show("/symlink_test/",sym_link_resolve_abs,true);

    Debug::printf("*** ----------------------------------------------------------------------------------\n");

    Debug::printf("*** TEST 3: resolving root symlinks\n");
    auto sym_link_no_resolve_root = fs->find(root,"dir1/rootSym");
    show("/dir1/rootSym",sym_link_no_resolve_root,false);
    auto sym_link_resolve_root = fs->find(root,"dir1/rootSym/");
    show("/",sym_link_resolve_root,true);

    Debug::printf("*** ----------------------------------------------------------------------------------\n");

    Debug::printf("*** TEST 4: looped relative path symlink\n");
    auto looped_absolute = fs->find(root,"/dir1/dir2/dir3/dir4/relativeRecurSym/dir2/dir3/dir4/relativeRecurSym/dir2/dir3/dir4/relativeRecurSym/dir2/dir3/dir4/relativeRecurSym/dir2/dir3/dir4/relativeRecurSym/dir2/dir3/dir4/relativeRecurSym/dir2/dir3/dir4/relativeRecurSym/dir2/dir3/dir4/relativeRecurSym/dir2/dir3/dir4/relativeRecurSym/dir2");
    show("dir1/dir2",looped_absolute,false);

    Debug::printf("*** ----------------------------------------------------------------------------------\n");

    Debug::printf("*** TEST 5: assert symlink node is equal to actual node in a trie\n");

    auto sym_link_gheith = fs->find(root,"symlinks/gheith/");
    auto sym_link_gheith_actual = fs->find(root,"trie/g/h/e/i/t/h");
    CHECK(sym_link_gheith->number == sym_link_gheith_actual->number);

    auto sym_link_green = fs->find(root,"symlinks/green/");
    auto sym_link_green_actual = fs->find(root,"trie/g/r/e/e/n");
    CHECK(sym_link_green->number == sym_link_green_actual->number);

    auto sym_link_game = fs->find(root,"symlinks/game/");
    auto sym_link_game_actual = fs->find(root,"trie/g/a/m/e");
    CHECK(sym_link_game->number == sym_link_game_actual->number);

    Debug::printf("*** passed test 5!\n");

    Debug::printf("*** ----------------------------------------------------------------------------------\n");










}

