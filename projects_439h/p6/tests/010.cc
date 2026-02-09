#include "ext2.h"
#include "ide.h"
#include "physmem.h"
#include "shared.h"
#include "vmm.h"

using namespace VMM;

// SUPER DUPER COOL MMAP TEST
//
// This test contains a few parts.
//
// 1. What happens when you attempt to `mmap` on a full region?
//      - You should return `nullptr` if you don't have space
//      - You should also be able to `munmap` on this correctly
//        and allow all that space to be reclaimed
//
// 2. What happens when you attempt to `mmap` _all_ of shared memory?
//      - You shouldn't be able to do this because you cannot write
//        over the lAPIC and ioPIC
//
// 3. Can your `mmap` be used to do cool things?
//      - Yeah, it can. If you believe. ;)
//      - You'll execute a pre-compiled binary that is mmap'd into memory
//      -  NOTE: if you want to see the source code, it, as well as the censored
//               build file are available in 010.dir

// FULL REGION MMAP TEST
//
// This test fills up the thread-private portion of the virtual address
// space, and then tries to `mmap` one more byte.
//
// The expected behavior (in spec) is to return `nullptr` if there is an
// error while mmap-ing. Then, if there is no space to give, you should
// return nullptr.
void full_region_mmap_test() {
  // This is the number of bytes needed to fill up the
  // thread-private region of the virtual address space
  constexpr uint32_t NUM_BYTES = 0xF0000000 - 0x80000000;

  // `mmap` the whole thing at once for now...
  // NOTE: Making this iterative instead can test your mmap efficiency,
  //       but I think that this is fine as is
  void *p = naive_mmap(NUM_BYTES, false, {}, 0);
  Debug::printf("p = 0x%x\n", p);
  ASSERT(p == (void *)0x80000000);

  // Now let's see what happens when we map one more!
  void *q = naive_mmap(1, false, {}, 0); // this should err -> nullptr
  Debug::printf("q = 0x%x\n", q);
  ASSERT(q == nullptr);

  // We need this space now...
  naive_munmap(p);

  Debug::printf("*** FINISHED `full_region_mmap_test()`\n");
}

// FULL SHARED REGION MMAP TEST
//
// We can try to allocate all of the shared region, but we
// should fail because it would involve overwriting the
// lAPIC and ioPIC
void full_shared_region_mmap_test() {
  // This is the number of bytes needed to fill up the
  // shared region of the virtual address space
  constexpr uint32_t NUM_BYTES = (0xFFFFFFFF - 0xF0000000 + 1);

  // Attempt to `mmap` the whole thing at once
  void *p = naive_mmap(NUM_BYTES, true, {}, 0);
  Debug::printf("p = 0x%x\n", p);
  ASSERT(p == nullptr);

  Debug::printf("*** FINISHED `full_shared_region_mmap_test()`\n");
}

// LOAD AN EXECUTABLE INTO MEMORY
// ... and then we can jump to it test
//
// There's nothing special you have to implement here. It should just
// work, but demonstrate a cool usecase for `mmap`.
//
// Of course, our `mmap` is quite bad at this. Loading elf files
// becomes hard... But to do something simple, it's perfectly suitable.
//
// And here's the actual test code...
void do_something_interesting_test() {
  // We will want a buffer for this portion of the test,
  // so let's just `mmap` a page
  void *buffer = naive_mmap(PhysMem::FRAME_SIZE, false, {}, 0);
  ((char *)buffer)[0] = 'a';

  // Now we actually need the file system for something...
  //
  // So we can init our fs and grab the root
  StrongPtr<Ide> ide = StrongPtr<Ide>::make(1, 0); // who needs realism?
  StrongPtr<Ext2> fs = StrongPtr<Ext2>::make(ide);
  StrongPtr<Node> root = fs->root;

  // Now we can load our binary into memory
  StrongPtr<Node> thing = fs->find(root, "./int");
  void *r = naive_mmap(thing->size_in_bytes(), false, thing, 0);
  ASSERT(r == (void *)0x80001000);

  // Call it
  Debug::printf("CALLING...\n");
  typedef void func();
  func *f = (func *)0x80002000;
  f();

  // What's in the buffer?
  Debug::printf("*** %s\n", (char *)buffer);
  Debug::printf("*** FINISHED `do_something_interesting_test()`\n");
}

// Not interesting...
void kernelMain() {
  // VM on
  Debug::printf("*** VM BAGELZ\n");

  // Do full private region test
  Debug::printf("*************\n");
  full_region_mmap_test();

  // Do full shared region test
  Debug::printf("*************\n");
  full_shared_region_mmap_test();

  // Do something interesting test
  Debug::printf("*************\n");
  do_something_interesting_test();

  // Done! Well done!
  Debug::printf("*************\n");
  Debug::printf("*** よくできましたね〜\n");
}
