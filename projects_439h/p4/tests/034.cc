#include "debug.h"
#include "threads.h"
#include "shared.h"

#define chk_10(X) if(X->val != 10) Debug::panic("Failed chk_10 at the current line!\n")


class Data {
  public:
  int val;
  Data(int v) {
    val = v;
  }
};

/**
 * This tests a specific issue I encountered when
 * debugging my code, where deletion of a strong ptr 
 * can sometimes wrongly propogate to the below data.
 * I hope this test case can be beneficial in uncovering
 * this specific issue, without having to debug others.
 *
 * Because we use macros, a failure will be shown
 * at the line number that it happened. Useful!
 * 
 * A failure in your code is likely in your 
 * deconstructor. Check there first.
*/
void kernelMain(void) {
    Debug::printf("*** Starting\n");

    // Deleting doesn't propogate
    {
      StrongPtr<Data> inside { new Data(10) };
      chk_10(inside);
      {
        StrongPtr<StrongPtr<Data>> outside = StrongPtr<StrongPtr<Data>>::make(inside);
      }
      chk_10(inside);
      Debug::printf("*** Deleting doesn't propogate\n");
    }
    
    // Deleting doesn't propogate, weak
    {
      StrongPtr<Data> data { new Data(10) }; 
      StrongPtr<StrongPtr<Data>> ptr = StrongPtr<StrongPtr<Data>>::make(data);
      chk_10(data);
      {
       WeakPtr<StrongPtr<Data>> weak {ptr}; 
      }
      chk_10(data);
      Debug::printf("*** Deleting doesn't propogate, weak\n");
    }
    

    Debug::printf("*** Yay! Good job\n");
}

