#include "atomic.h"
#include "barrier.h"
#include "debug.h"
#include "random.h"
#include "shared.h"
#include "threads.h"

class ptrWrapper {

public:
  StrongPtr<ptrWrapper> ptr{};
  ptrWrapper() {}
};

void kernelMain(void) {
  uint32_t N = 30;
  StrongPtr<ptrWrapper> ring_head;
  StrongPtr<ptrWrapper> temp_head;
  Barrier *ring_done = new Barrier(2);
  Atomic<uint32_t> ring_size{1};

  Debug::printf("*** test 1: circular dependencies of strong pointers\n");

  Atomic<bool> ringDone{false};
  thread([&ring_head, &temp_head, &ring_size, ring_done, N]() -> void {
    ring_head = StrongPtr<ptrWrapper>{new ptrWrapper{}};
    temp_head = ring_head;
    while (ring_size.add_fetch(1) < N) {
      // Debug::printf("*** incremented to %d\n", ring_size.get());
      temp_head->ptr = StrongPtr<ptrWrapper>{new ptrWrapper{}};
      temp_head = temp_head->ptr;
    }
    temp_head->ptr = ring_head;
    ring_done->sync();
  });
  ring_done->sync();
  // Debug::printf("*** final is %d\n", ring_size.get());

  Barrier *t1Barrier2 = new Barrier(11);
  Debug::printf("*** part 1 dummy threads: expect 10 asterisks");
  for (int i = 0; i < 10; i++) {
    thread([t1Barrier2]() -> void {
      Debug::printf("*");
      t1Barrier2->sync();
    });
  }
  t1Barrier2->sync();
  Debug::printf("\n");

  ASSERT(ring_size.get() == N);
  ASSERT(ring_head != nullptr);
  temp_head->ptr = nullptr;
  Debug::printf("*** test 1 passed!\n");
  return;
}
