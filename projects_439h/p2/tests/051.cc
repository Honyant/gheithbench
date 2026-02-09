#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"


/* Called by one core */
void kernelMain(void) {

    // int answer = 0;
    auto b = new Barrier(5 * 50 + 1);

    for (int i = 0; i < 50; i++) {
        thread([b] {
            auto answer1 = new Promise<int>();
            thread([answer1, b] {
                auto answer2 = new Promise<int>();
                thread([answer2, b] {
                    auto answer3 = new Promise<int>();
                    thread([answer3, b] {
                        auto answer4 = new Promise<int>();
                        thread([answer4, b] {
                            answer4->set(1);
                            b->sync();
                        });
                        answer3->set(answer4->get() * 2);
                        b->sync();
                    });
                    answer2->set(answer3->get() * 2);
                    b->sync();
                });
                answer1->set(answer2->get() * 2);
                b->sync();
            });
            int answer = answer1->get() * 2;
            Debug::printf("*** answer = %d\n", answer);
            b->sync();
        });   
    }
    b->sync();
    Debug::printf("*** done yepee!\n");
}