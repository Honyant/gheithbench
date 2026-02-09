#include "barrier.h"
#include "config.h"
#include "debug.h"
#include "ext2.h"
#include "ide.h"
#include "libk.h"
#include "promise.h"
#include "shared.h"
#include "vmm.h"

// This is an efficiency test for your identity and shared mapping impl.

/* Identity Mapping */

// In a good identity mapping implementation, a thread will only allocate 1 page
// frame.
// This page frame should simply copy over the page directory entries of some
// global identity mapping.

// In a bad identity mapping implementation, a thread will allocate 33 pages.
// This is because the thread will create 1 page frame for the page directory,
// and 32 page frames for 32 page tables.

// This test ensures that you are conservative with your page allocation.
// If you greedily allocate 33 pages for each thread rather than 1 for the
// identity mapping, you will run out of frames for the large file I allocate.

// This will likely happen when you begin page faulting and allocating pages as
// I demand them.

/* Shared Mapping */

// In a good shared mapping implementation, all of the page tables will be
// shared between threads.

// In a bad shared mapping implementation, each thread will have its own copy
// of the page tables. This means that each thread will potentially even
// allocate frivolous data pages.

// This test ensures that you share page tables by allocating a large
// shared region and writing to it from multiple threads.

// If you greedily allocate new page tables and combinatorically explode
// the number of data pages you allocate, then you will run out of frames.

uint32_t FRAME_TOTAL = kConfig.memSize;
uint32_t IDEN_SIZE = FRAME_TOTAL - (1700 * PhysMem::FRAME_SIZE);
uint32_t SHARED_SIZE = 10 * 1024 * 1024; // 10 MB

constexpr uint32_t N = 10;
// 25 * 10 MB = 250 MB which is way too many frames!!!
// So you really shouldn't be allocating new page tables for each thread :3
constexpr uint32_t M = 25;

Atomic<uint32_t> iden_count{0};
Atomic<uint32_t> shared_count{0};

void stress_identity_mapping_test() {
  using namespace VMM;

  Debug::printf("*** Identity Mapping Test\n");

  Barrier *b = new Barrier(N + 1);

  // Spawn ten threads that increment the count
  for (uint32_t i = 0; i < N; i++) {
    thread([b] {
      iden_count.fetch_add(1);
      b->sync();
    });
  }

  // Wait for threads to complete
  while (iden_count.get() < N) {
    yield();
  }

  CHECK(iden_count.get() == N);

  void *large_file = naive_mmap(IDEN_SIZE, false, {}, 0);
  CHECK(large_file == (char *)0x80000000);
  // Force you to allocate data pages
  for (uint32_t i = 0; i < IDEN_SIZE; i++) {
    ((char *)large_file)[i] = 'a';
    ASSERT(((char *)large_file)[i] == 'a');
  }

  // Sync to make sure we wait for each thread to complete its work :)
  b->sync();

  // Clean up
  naive_munmap(large_file);
}

void stress_shared_mapping_test() {
  using namespace VMM;

  Debug::printf("*** Shared Mapping Test\n");

  void *shared_file = naive_mmap(SHARED_SIZE, true, {}, 0);
  CHECK(shared_file == (char *)0xF0000000);

  Barrier *b = new Barrier(M + 1);

  // Force you to allocate data pages
  for (uint32_t i = 0; i < M; i++) {
    thread([shared_file, b] {
      shared_count.fetch_add(1);
      // if (shared_count.get() % 10 == 0) {
      //   // How far did you get?
      //   Debug::printf("%d\n", shared_count.get());
      // }
      for (uint32_t i = 0; i < SHARED_SIZE; i++) {
        ((char *)shared_file)[i] = 'a';
        ASSERT(((char *)shared_file)[i] == 'a');
      }
      b->sync();
    });
  }

  // Wait for threads to complete
  while (shared_count.get() < M) {
    yield();
  }

  CHECK(shared_count.get() == M);

  // Sync to make sure we wait for each thread to complete its work :)
  b->sync();

  // Clean up - this won't do anything because it's shared though :o
  naive_munmap(shared_file);
}

void kernelMain(void) {
  Debug::printf("*** FRAME_TOTAL = %x\n", FRAME_TOTAL);         // 0x7fe0000
  Debug::printf("*** IDEN_SIZE = %x\n", IDEN_SIZE);             // 0x793c000
  Debug::printf("*** SHARED_SIZE = %x = 10 MB\n", SHARED_SIZE); // 0xec00000

    int64_t a = 0x70000011;
    int64_t b = 0x70000011;

    Debug::printf("a: %lx\n", a);
    Debug::printf("b: %lx\n", b);
    Debug::printf("axb: %lx", (a*b) >> 32);
    Debug::printf("%lx\n", (a*b) & 0xFFFFFFFF);

    Debug::printf("Size of int: %d bytes\n", sizeof(int));
    Debug::printf("Size of long: %d bytes\n", sizeof(int64_t));

  // If you can pass both of these tests individually, but not together
  // Then your thread destructor is probably incorrect :(
  stress_identity_mapping_test();
  stress_shared_mapping_test();

  Debug::printf("*** Test Complete!\n");
}
