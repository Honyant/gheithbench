#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "barrier.h"

// This is a test of shared memory and some of the weird stuff that can go on with it!
// IMPORTANT NOTE: MunMap is a noop on shared memory according to the spec, so these test cases parts unfortuantely
// are not completely independent from eachother. I have done my best to make them as self contained as possible.
// Also, to avoid confusion, each file is named [character]File and is just a file containing that character

/*template <typename T, typename Work>
inline StrongPtr<Promise<T>> future(Work const& work) {
    auto p = StrongPtr<Promise<T>>::make();
    thread([p, work] {
        p->set(work());
        Debug::printf("Stopping for fun and profit\n");
    });
    return p;
}*/

/* Called by one CPU */
void kernelMain(void) {

    using namespace VMM;


    // Part 0: initialize IDE and file system
    Debug::printf("*** Part 0: initialize IDE and file system\n");
    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 1);

    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);

    // get "/"
    auto root = fs->root;

    Debug::printf("*** Part 0 complete\n***\n");

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // Part 1: simple mmap of shared memory
    Debug::printf("*** Part 1: simple mmap of shared memory\n");

    //  get "/aFile"
    auto pt1AFile = fs->find(root,"aFile");

    auto pt1AFileData = (char*) naive_mmap(pt1AFile->size_in_bytes(),true,pt1AFile,0);
    Debug::printf("*** %s\n",pt1AFileData);

    Debug::printf("*** Part 1 complete\n***\n");

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // Part 2: two mmaps of shared memory, to make sure that we are not overwriting old memory
    Debug::printf("*** Part 2: two mmaps of shared memory, to make sure that we are not overwriting old memory\n");

    //  get "/bFile"
    auto pt2BFile = fs->find(root,"bFile");

    //  get "/cFile"
    auto pt2CFile = fs->find(root,"cFile");

    // Map both files
    auto pt2BFileData = (char*) naive_mmap(pt2BFile->size_in_bytes(),true,pt2BFile,0);
    auto pt2CFileData = (char*) naive_mmap(pt2CFile->size_in_bytes(),true,pt2CFile,0);
    
    // Print data
    Debug::printf("*** %s\n",pt2BFileData);
    Debug::printf("*** %s\n",pt2CFileData);

    Debug::printf("*** Part 2 complete\n***\n");

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // Part 3: confirm that munmap is a noop on shared memory
    Debug::printf("*** Part 3: confirm that munmap is a noop on shared memory\n");

    //  get "/dFile"
    auto pt3DFile = fs->find(root,"dFile");

    //  get "/eFile"
    auto pt3EFile = fs->find(root,"eFile");

    // Map the first file then unmap it
    auto pt3DFileData = (char*) naive_mmap(pt3DFile->size_in_bytes(),true,pt3DFile,0);
    naive_munmap(pt3DFileData);

    // Map the second file
    auto pt3EFileData = (char*) naive_mmap(pt3EFile->size_in_bytes(),true,pt3EFile,0);
    
    // Print data
    Debug::printf("*** %s\n",pt3DFileData);
    Debug::printf("*** %s\n",pt3EFileData);

    Debug::printf("*** Part 3 complete\n***\n");

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // Part 4: confirm shared memory is actually shared between threads!
    Debug::printf("*** Part 4: confirm shared memory is actually shared between threads!\n");

    //  get "/fFile"
    auto pt4FFile = fs->find(root,"fFile");


    // Map the file
    auto pt4FFileData = (char*) naive_mmap(pt4FFile->size_in_bytes(),true,pt4FFile,0);

    // Create a barrier
    Barrier * pt4Bar = new Barrier(2);

    // Read the file in the other thread
    thread([pt4Bar, pt4FFileData] {
        // These prints can be useful for debugging
        // Debug::printf("*** Entered pt4 thread\n");
        Debug::printf("*** %s\n",pt4FFileData);
        pt4Bar->sync();
    });

    pt4Bar->sync();

    Debug::printf("*** Part 4 complete\n***\n");

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // Part 5: confirm synchronization of reads!
    Debug::printf("*** Part 5: confirm synchronization of reads!\n");

    //  get "/gFile"
    auto pt5GFile = fs->find(root,"gFile");

    // Map the file
    auto pt5GFileData = (char*) naive_mmap(pt5GFile->size_in_bytes(),true,pt5GFile,0);

    // Create a barrier
    Barrier * pt5Bar = new Barrier(11);

    // Read the file in the other thread
    for(int i = 0; i < 10; i++){
        thread([pt5Bar, pt5GFileData] {
            // These prints can be useful for debugging
            // Debug::printf("*** Entered pt5 thread\n");
            char shouldBeG = *((char *)(pt5GFileData));
            ASSERT(shouldBeG == 'g');
            pt5Bar->sync();
        });
    }

    pt5Bar->sync();

    Debug::printf("*** %s\n",pt5GFileData);

    Debug::printf("*** Part 5 complete\n***\n");
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // Part 5: confirm synchronization of reads!
    Debug::printf("*** Part 6: confirm synchronization of reads!\n");

    //  get "/hFile"
    auto pt6HFile = fs->find(root,"hFile");

    // Map the h file
    auto pt6HFileData = (char*) naive_mmap(pt6HFile->size_in_bytes(),true,pt6HFile,0);

    //  get "/iFile"
    auto pt6IFile = fs->find(root,"iFile");

    // Map the i file
    auto pt6IFileData = (char*) naive_mmap(pt6IFile->size_in_bytes(),true,pt6IFile,0);

    // Create a barrier
    Barrier * pt6Bar = new Barrier(11);

    // Each thread will map the h or i file and then confirm that the data read from it was correct
    // If any of these fail, you may have some race condition in your mapping of shared memory
    for(int i = 0; i < 10; i++){
        thread([pt6Bar, pt6IFile, pt6HFile, i] {
            // These prints can be useful for debugging
            // Debug::printf("*** Entered pt6 thread\n");
            
            char * pt6ThreadFileData;
            if( i % 2 == 0){
                pt6ThreadFileData = (char*) naive_mmap(pt6IFile->size_in_bytes(),true,pt6IFile,0);
            } else{
                pt6ThreadFileData = (char*) naive_mmap(pt6HFile->size_in_bytes(),true,pt6HFile,0);
            }

            if(i%2 == 0){
                char shouldBeI = *((char *)(pt6ThreadFileData));
                ASSERT(shouldBeI == 'i');
            } else{
                char shouldBeH = *((char *)(pt6ThreadFileData));
                ASSERT(shouldBeH == 'h');
            }
            pt6Bar->sync();
        });
    }

    pt6Bar->sync();

    Debug::printf("*** %s\n",pt6HFileData);
    Debug::printf("*** %s\n",pt6IFileData);

    Debug::printf("*** Part 6 complete\n");

    ////////////////////////////////////////////////////////////////////////////////////////////////////
}
