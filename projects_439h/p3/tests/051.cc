#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"
#include "bb.h"

/* Called by one core */
void kernelMain(void) {
    // pass answer through an list of promises
    int arr_size = 100;
    auto b1 = new Barrier(arr_size);

    Promise<int>** prom_arr = (Promise<int>**) malloc(arr_size*sizeof(Promise<int>));
    for(int i = 0; i < arr_size; i++) {
        prom_arr[i] = new Promise<int>();
    }
    prom_arr[arr_size-1]->set(42);
    for(int i = 1; i < arr_size; i++) {
        thread([i, prom_arr, b1] {
            prom_arr[i-1]->set(prom_arr[i]->get());
            b1->sync();
        });
    }
    b1->sync();
    Debug::printf("*** answer = %d\n",prom_arr[0]->get());
    free(prom_arr);

    // pass answers from one list of promises to other list of promises
    int arr_size2 = 26;
    auto b2 = new Barrier(arr_size2+1);

    Promise<char>** from_arr = (Promise<char>**) malloc(arr_size2*sizeof(Promise<char>));
    Promise<char>** to_arr = (Promise<char>**) malloc(arr_size2*sizeof(Promise<char>));
    Debug::printf("***");

    for(int i = 0; i < arr_size2; i++) {
        from_arr[i] = new Promise<char>();
        to_arr[i] = new Promise<char>();
    }

    for(int i = 0; i < arr_size2; i++) {
        thread([i, from_arr, to_arr, b2] {
            to_arr[i]->set(from_arr[i]->get());
            b2->sync();
            
        });
        from_arr[i]->set(65+i);

        Debug::printf("%c",to_arr[i]->get());
    }

    b2->sync();
    Debug::printf("\n");

    free(from_arr);
    free(to_arr);
    // bounded buffer test - should only get first value (1)
    auto bb1 = new BB<int>(2);
    auto b3 = new Barrier(2);
    int val1 = 1;
    int val2 = 2;
    thread([bb1, b3, &val1, &val2] {
        bb1->put(val1);
        bb1->put(val2);
        b3->sync();
    });
    b3->sync();
    Debug::printf("*** got value %d\n",bb1->get());


    // bounded buffer and sleep test - should only get first value (2)
    auto bb2 = new BB<int>(2);
    auto b4 = new Barrier(3);

    thread([bb2, b4, &val1] {
        sleep(2);
        bb2->put(val1);
        b4->sync();
    });

    thread([bb2, b4, &val2] {
        sleep(1);
        bb2->put(val2);
        b4->sync();
    });

    b4->sync();
    Debug::printf("*** got value %d\n",bb2->get());
    
}
