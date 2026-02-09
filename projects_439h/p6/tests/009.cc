#include "config.h"
#include "ext2.h"
#include "ide.h"
#include "libk.h"
#include "shared.h"
#include "vmm.h"
/* Called by one CPU */
void kernelMain(void) {

  // this test ensures that you properly mmap files with multiple blocks,
  // that you can properly allocate pages beyond the first block,
  // and that you can offset a file more than a block.
  // It also ensure that you can mmap multiple times from the same file

  // file system initialization from t0
  using namespace VMM;

  // IDE device #1
  auto ide = StrongPtr<Ide>::make(1, 1);

  // We expect to find an ext2 file system there
  auto fs = StrongPtr<Ext2>::make(ide);

  ASSERT(fs != nullptr);
  // get "/"
  auto root = fs->root;

  //  get "/hello"
  auto otherdir = fs->find(root, "/dir0/dir1/dir2/dir3/");

  auto bigFile = fs->find(otherdir, "bigFile.txt");

  // massive mmap size ensures that you only allocate frames on demand
  char *hiString = (char *)naive_mmap(kConfig.memSize, false, bigFile, 6171);

  // add string termination character
  hiString[4] = 0;

  // the first string should be empty, because pages beyond
  // the end of the file should be filled with zeroes.
  Debug::printf("*** %s%s", hiString + 0x4000000, hiString);

  char *wholeFile = (char *)naive_mmap(14000, false, bigFile, 0);

  // add string termination character
  wholeFile[12277 + 41 + 185] = 0;

  Debug::printf("%s", wholeFile + 12277 + 41);

  // for
  // debugging purposes :
  // this is equivalent code reading directly from the
  // filesystem.

  // char buffer[5];
  // bigFile->read_all(6171, 4, buffer);
  // buffer[4] = 0;
  // Debug::printf("*** %s", buffer);

  // char *buffer2 = new char[186];
  // bigFile->read_all(12277 + 41, 186, buffer2);
  // buffer2[185] = 0;
  // Debug::printf("%s", buffer2);
}
