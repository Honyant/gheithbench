#include "debug.h"
#include "threads.h"
#include "bb.h"


constexpr int N = 100;

/* Called by one core */
void kernelMain(void) {

    auto bb = new BB<int>(1);

    
    thread([bb] {
        for (int i=0; i<N; i++) {
            bb->put(i);
        }
    });
    
    int last = bb->get();
    ASSERT(last == 0);

    while(last < N-1) {
        auto expected = last+1;
        last = bb->get();
        if (last != expected) {
            Debug::panic("*** expected:%d last:%d\n", expected, last);
        }
    }

    Debug::printf("*** all good %d\n", last);

}

