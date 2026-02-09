#include "atomic.h"
#include "barrier.h"
#include "debug.h"
#include "idt.h"
#include "pit.h"
#include "shared.h"
#include "smp.h"

// Disclaimer: I have taken the interrupt handler code from ta0

// Explanation of TC:
// 1.
// This test case tests a few simple reference counting cases for smart pointers

// 2.
// This test case will divide by 0, which should jump to a custom divide by 0
// handler.
// You do not need to do anything special in your implementation for
// the new handler. If you fail, you may have somehow disabled
// interrupts/exception handling.

// 3.
// This test case ensures that sleep() doesn't block the core.
// It should try to run another process while sleeping.

// 4.
// This test case ensures that your barrier implementation still works!

struct BAGEL {
  int numFriends;

  BAGEL(int numFriends) : numFriends(numFriends) {}

  ~BAGEL() {
    Debug::printf(
        "*** Bagel Destructed | (Bagel was shared with %d friends!) \n",
        numFriends);
  }
};

void momSplitBagelsAmongFriends(StrongPtr<BAGEL> bagel) {
  volatile uint32_t a = 3;
  volatile uint32_t num_friends = 1; // Your mom is your friend
  if (bagel != nullptr) {
    num_friends = bagel->numFriends;
    Debug::printf(
        "*** Bagel Not Null | Friends %d | (You don't share with your "
        "mom :o ...) \n",
        num_friends);
  } else {
    Debug::printf(
        "*** Bagel Null | (Such a sweetie! Sharing bagels with your mom!) \n");
  }
  a = a / num_friends;
}

void splitBagelsAmongSiblings(StrongPtr<BAGEL> bagel) {
  // 10/26 is a special bagel person's birthday - do you know who?
  volatile uint32_t a = 1026;        // Number of bagels
  volatile uint32_t num_friends = 0; // Default number of friends
  if (bagel != nullptr) {
    // Uh oh... do you see a problem with having 0 friends?
    num_friends = bagel->numFriends;
  }
  // This should trigger a divide by 0 exception
  a = a / num_friends;
}

void testWeakPtrPromotion() {
  // Test 1: WeakPtr and null promotion
  Debug::printf("*** Test 1\n");
  StrongPtr<BAGEL> bagel4{new BAGEL(9)};
  WeakPtr<BAGEL> rotten_bagel{bagel4};

  StrongPtr<BAGEL> bagel_with_schmear = rotten_bagel.promote();
  momSplitBagelsAmongFriends(bagel_with_schmear); // Bagel Not Null | Friends

  bagel4 = nullptr; // Bagel Destructed
  bagel_with_schmear = nullptr;
  StrongPtr<BAGEL> bagel_without_schmear = rotten_bagel.promote();
  ASSERT(bagel_without_schmear == nullptr);        // The promotion should fail
  splitBagelsAmongSiblings(bagel_without_schmear); // Divide by 0
}

void testStrongPtrScope() {
  // Test 2: StrongPtr goes out of scope and becomes a nullptr
  Debug::printf("*** Test 2\n");
  StrongPtr<BAGEL> bagel1{new BAGEL(5)};
  momSplitBagelsAmongFriends(bagel1); // Bagel Not Null | Friends
  bagel1 = nullptr;                   // Bagel Destructed
  momSplitBagelsAmongFriends(bagel1); // Bagel Null
}

void testSharedOwnership() {
  // Test 3: Shared ownership with StrongPtr
  Debug::printf("*** Test 3\n");
  StrongPtr<BAGEL> bagel2;
  {
    StrongPtr<BAGEL> bagel3{new BAGEL(9)};
    momSplitBagelsAmongFriends(bagel2); // Bagel Null
    momSplitBagelsAmongFriends(bagel3); // Bagel Not Null | Friends
    bagel2 = bagel3;
    momSplitBagelsAmongFriends(bagel2); // Bagel Not Null | Friends
    momSplitBagelsAmongFriends(bagel3); // Bagel Not Null | Friends
  }
  momSplitBagelsAmongFriends(bagel2); // Bagel Not Null | Friends
} // Bagel Destructed

extern "C" void divideBy0HandlerAsm();

void kernelMain(void) {
  // Source for arithmetic_exception_vector being entry 0
  // https://en.wikipedia.org/wiki/Interrupt_descriptor_table#:~:text=The%20interrupt%20descriptor%20table%20(IDT,executed%20on%20interrupts%20and%20exceptions.
  // This ensures that divide by 0 exceptions will route to my custom
  // divideBy0Handler
  IDT::interrupt(0, (uint32_t)divideBy0HandlerAsm);

  testWeakPtrPromotion();
}

extern "C" void apitHandler(uint32_t *things);

extern "C" void divideBy0Handler(uint32_t *things) {
  StrongPtr<Barrier> b{new Barrier(200 + 1)};

  // Spawn a few sleeping threads for fun!
  // This shouldn't increase the time it takes to pass the test case as you
  // should block in sleep rather than spin!
  for (int i = 0; i < 200; i++) {
    thread([b]() mutable {
      b->sync();
      sleep(100);
      Debug::printf("*** Failure | (You should never sleep 100...) \n");
    });
  }
  b->sync();

  Debug::printf("*** Divide By 0 | (You already ate this bagel...) \n");
  apitHandler(things);

  testStrongPtrScope();
  testSharedOwnership();

  Debug::printf("*** Shut Down \n");
  Debug::shutdown();
}

asm(R"""(
divideBy0HandlerAsm:
    pusha
    push %esp
    call divideBy0Handler
    pop %esp
    popa
    iret
)""");