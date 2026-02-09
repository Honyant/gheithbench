#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"

const char* file_contents = "let me tell you about my favorite class, opearting systems with dr. ahmed gheith. Wow, what can I say. I'm just always entranced by the wonderful lectures and challenging projects. Every time I get an email with all the test cases I'm failing, I'm pushed to work harder and learn more about computer science. Oh, operating systems my operating systems, how I want you for all my life. I will forever be sad when I must leave operating systems, but forever grateful for the lovely concepts taught.\0"; 

//void streq_with_size(char* test, uint32_t offset, uint32_t 

/**
 * This test case tests the offset field for mmap.
 * It loops over the size of the file to make sure, given the offset, 
 * the starting index is correct for the file content.
 * Make sure you are carefully considering how your offset is being stored!
 */
void kernelMain(void) {
  using namespace VMM;

  auto ide = StrongPtr<Ide>::make(1, 1);
  auto fs = StrongPtr<Ext2>::make(ide);
  auto root = fs->root;
  auto hi = fs->find(root,"hi.txt");
  Debug::printf("*** wow your implementaton is so good! let's test it further...\n");
  uint32_t length = K::strlen(file_contents);
  
  for(uint32_t offset = 0; offset < length; offset++) {
    auto p = (char*) naive_mmap(1, false, hi, offset);
    ASSERT(p[0] == file_contents[offset]);
  }
  Debug::printf("*** U are super smart wow!!!\n");


  
}
