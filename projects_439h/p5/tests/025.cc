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


void kernelMain(void) {
    auto ide = StrongPtr<Ide>::make(1, 3);
    auto fs = StrongPtr<Ext2>::make(ide);

    //first, we find the root of the file system
    auto root = fs->root;
    show("/",root,true);
    
    //next, we start with finding files and directories from the root.
    auto myFile = fs->find(root,"myFile.txt");
    show("/myFile.txt",myFile,true);
    
    auto level1 = fs->find(root,"/level1");
    show("/level1",level1,true);

    //testing that we can perform finds of arbitrary depth from a directory
    auto level2 = fs->find(root,"/level1/level2");
    show("/level1/level2",level2,true);
    
    auto level3 = fs->find(root,"/level1/level2/level3");
    show("/level1/level2/level3",level3,true);

    //testing that we find the correct file within an arbitrary depth search from find
    auto level1File = fs->find(root,"/level1/insideLevel1.txt");
    show("/level1/insideLevel1",level1File,true);

    auto level2File = fs->find(root,"/level1/level2/insideLevel2.txt");
    show("/level1/level2/insideLevel2",level2File,true);

    auto level3File = fs->find(root,"/level1/level2/level3/insideLevel3.txt");
    show("/level1/level2/level3/insideLevel3",level3File,true);

    //testing that we can perform a find to a higher level directory from the find function
    auto myFileBack = fs->find(level1, "../myFile.txt");
    show("level1/../myFile.txt",myFileBack,true);

    //testing parsing of . and .. files in the directory and confirming that the correct file is found
    //testing that having both .. and . in the same path does not cause issues
    auto myFileBackDot = fs->find(level1, ".././myFile.txt");
    show("level1/.././myFile.txt",myFileBackDot,true);

    auto myFileBackLevel2 = fs->find(level2, "../insideLevel1.txt");
    show("level2/../insideLevel1.txt",myFileBackLevel2,true);

    //this should give back the current directory
    auto dotTestDir = fs->find(level2, "././././././././././././.");
    show("level2/././././././././././././.",dotTestDir,true);

    //confirming that this gives back the current directory by reading the contents of a file stored there
    auto dotTestFile = fs->find(level2, "./././././././././././././insideLevel2.txt");
    show("level2/./././././././././././././insideLevel2.txt",dotTestFile,true);

    //testing combining ., .., and a directory into the path of find
    auto dotTestFileEnding = fs->find(level2, "./././././././././././././../level2/./././././././././././././");
    show("./././././././././././././../level2/./././././././././././././",dotTestFileEnding,true);

    //testing traversal back up the file hierarchy, combining . and ..
    auto combinedDotTest = fs->find(level3, "./.././.././insideLevel1.txt");
    show("./.././.././insideLevel1.txt",combinedDotTest,true);

    //symlink test
    auto symLinkTest1 = fs->find(level3, "symLinkToLevel1");
    show("symLinkToLevel1",symLinkTest1,true);

    //the symlink is the last item in the path, so it should not be traversed. This should find the symlink itself, not
    //the file being symlinked to
    auto symLinkTest2 = fs->find(level2, "level3/symLinkToLevel1");
    show("level3/symLinkToLevel1",symLinkTest2,true);

    //this tests that symlinks are followed when they are not the last item in the path.
    //tests that the symlink to a different directory is followed by finding a file there
    auto symLinkTest3 = fs->find(level3, "symLinkToLevel1/insideLevel1.txt");
    show("symLinkToLevel1/insideLevel1.txt",symLinkTest3,true);

    //tests for resolution of multiple symlinks in a path. Each symlink should be resolved. 
    auto symLinkTest4 = fs->find(level1, "level2/level3/symLinkToLevel1/level2/level3/symLinkToLevel1/level2/insideLevel2.txt");
    show("level2/level3/symLinkToLevel1/level2/level3/symLinkToLevel1/level2/insideLevel2.txt",symLinkTest4,true);

    //a comprehensive test of many features:
    //arbitrary length find from a non-root directory.
    //path contains . and .. next to symlinks and each other. We should still have correct resolution of symlinks and traversal of file system
    //two different intermediate symlinks in the path that need to be resolved to correctly find the file 
    auto symLinkTest5 = fs->find(level1, "level2/level3/./symLinkToLevel1/level2/.././symLinkToLevel3/./insideLevel3.txt");
    show("level2/level3/./symLinkToLevel1/level2/.././symLinkToLevel3/./insideLevel3.txt",symLinkTest5,true);

}

