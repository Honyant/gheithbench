#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

/* Called by one core */
void kernelMain(void) {
    // pass answer through an list of promises
    int arr_size = 100;
    Promise<int>** prom_arr = (Promise<int>**) malloc(arr_size*sizeof(Promise<int>));
    for(int i = 0; i < arr_size; i++) {
        prom_arr[i] = new Promise<int>();
    }
    prom_arr[arr_size-1]->set(42);
    for(int i = 1; i < arr_size; i++) {
        thread([i, prom_arr] {
            prom_arr[i-1]->set(prom_arr[i]->get());
        });
    }
    Debug::printf("*** answer = %d\n",prom_arr[0]->get());
    free(prom_arr);

    // pass answers from one list of promises to other list of promises
    int arr_size2 = 26;
    Promise<char>** from_arr = (Promise<char>**) malloc(arr_size2*sizeof(Promise<char>));
    Promise<char>** to_arr = (Promise<char>**) malloc(arr_size2*sizeof(Promise<char>));
    Debug::printf("***");

    for(int i = 0; i < arr_size2; i++) {
        from_arr[i] = new Promise<char>();
        to_arr[i] = new Promise<char>();
        thread([i, from_arr, to_arr] {
            to_arr[i]->set(from_arr[i]->get());
        });
        from_arr[i]->set(65+i);
        Debug::printf("%c",to_arr[i]->get());
    }

    free(from_arr);
    free(to_arr);
}
