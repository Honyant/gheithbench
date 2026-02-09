#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

//show() taken from t0
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
        Debug::printf("***    is of type %04x\n",node->get_type());
    }
}

/*a bunch of tests for edge cases in find and symlinks*/
//note that you will have to run make clean before rerunning this test case because it contains absolute and invalid symlinks!
void kernelMain(void) {
    int testNum = 1;
    auto ide = StrongPtr<Ide>::make(1, 3);
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());
   
    //test 1 - finding root directory
    Debug::printf("*** \n");
    Debug::printf("*** test %d - getting root dir\n",testNum++);
    auto root = fs->root;
    show("/",root,true);

    //test 2 - finding normal file
    Debug::printf("*** \n");
    Debug::printf("*** test %d - basic file find\n",testNum++);
    auto hello = fs->find(root,"hello");
    show("hello",hello,true);

    //test 3 - nested find
    Debug::printf("*** \n");
    Debug::printf("*** test %d - nested file find\n",testNum++);
    auto doomed = fs->find(root,"/everything/is/../is/more/beautiful/because/we/are/doomed");
    show("everything/is/more/beautiful/because/we/are/doomed",doomed,true);

    //test 4 - find with symlink at end of path  -> should return symlink
    Debug::printf("*** \n");
    Debug::printf("*** test %d - symlink at end of path\n",testNum++);
    //endsymlink points to /test4/symlink/at/endTarget
    auto endSymlink = fs->find(root,"test4/symlink/at/end");
    CHECK(endSymlink != nullptr);
    CHECK(endSymlink->is_symlink());
    show("/test4/symlink/at/end",endSymlink,true);

    //test 5 - find with symlink + slash at end of path -> should return dir
    Debug::printf("*** \n");
    Debug::printf("*** test %d - symlink + slash at end of path\n",testNum++);
    //endsymlink points to /hello
    auto endSymlinkTarget = fs->find(root,"middleRelative/");
    CHECK(endSymlinkTarget != nullptr);
    CHECK(endSymlinkTarget->is_dir());
    show("target of middleRelative",endSymlinkTarget,true);

    //test 6 - find with relative symlink in the middle of path
    Debug::printf("*** \n");
    Debug::printf("*** test %d - relative symlink in middle of path\n",testNum++);
    //middlesymlink points to /everything/is/more/beautiful/
    auto middleSymlink = fs->find(root,"middleRelative");
    CHECK(middleSymlink != nullptr);
    CHECK(middleSymlink->is_symlink());
    show("middleRelative",middleSymlink,true);
    //check that we can actually reach the file thru the symlink
    auto middleSymlinkTarget = fs->find(root,"middleRelative/because/we/are/doomed");
    CHECK(middleSymlinkTarget != nullptr);
    CHECK(middleSymlinkTarget->is_file());
    show("everything/is/more/beautiful/because/we/are/doomed",middleSymlinkTarget,true);
    
    //test 7 - find with absolute symlink in the middle of path
    Debug::printf("*** \n");
    Debug::printf("*** test %d - absolute symlink in middle of path\n",testNum++);
    //middlesymlink points to /everything/is/more/beautiful/
    middleSymlink = fs->find(root,"test4/symlink/at/middleAbsolute");
    CHECK(middleSymlink != nullptr);
    CHECK(middleSymlink->is_symlink());
    show("/test4/symlink/at/middleAbsolute",middleSymlink,true);
    //check that we can actually reach the file thru the symlink
    middleSymlinkTarget = fs->find(root,"test4/symlink/at/middleAbsolute/because/we/are/doomed");
    CHECK(middleSymlinkTarget != nullptr);
    CHECK(middleSymlinkTarget->is_file());
    show("everything/is/more/beautiful/because/we/are/doomed",middleSymlinkTarget,true);

    //test 8 - dangling symlink (points to a nonexistent target named "nonexistent")
    Debug::printf("*** \n");
    Debug::printf("*** test %d - dangling symlink\n",testNum++);
    auto invalid_symlink = fs->find(root, "invalid_symlink");
    CHECK(invalid_symlink != nullptr);
    CHECK(invalid_symlink->is_symlink());
    show("/invalid_symlink", invalid_symlink, true); //should still point to "nonexistent"
}

