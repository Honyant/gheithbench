#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "random.h"
#include "barrier.h"

constexpr int LIST_SIZE = 100;
int list[LIST_SIZE];

// We will be running a parallel bubble sort for fun!!

void kernelMain(void) {

    for (int i = 0; i < 100; i++) {
        list[i] = i + 1;
    }

    Barrier *b1 = new Barrier{2};

    thread([b1] {
        Random *random = new Random(100);
        for (int i = LIST_SIZE - 1; i > -1; i--) {
            int j = (random -> next()) % 100;
            int temp = list[j];
            Debug::printf("%d\n", temp);
            list[j] = list[i];
            list[i] = temp;
        }

        b1 -> sync();
    });

    b1 -> sync();

    Barrier *b2 = new Barrier{5};
    for (int i = 0; i < 4; i++) {
        thread([b2, i] {
            bool swapped;
            for (int j = (LIST_SIZE * i)/4; j < (LIST_SIZE * (i + 1)/4) - 1; j++) {
                swapped = false;
                for (int k = (LIST_SIZE * i)/4; k < (LIST_SIZE * (2 * i + 1))/4 - 1 - j; k++) {
                    if (list[k] > list[k + 1]) {
                        int temp = list[k];
                        list[k] = list[k + 1];
                        list[k + 1] = temp;
                        swapped = true;
                    }
                }
                if (swapped == false)
                    break;
            }

            b2 -> sync();
        });
    }

    b2 -> sync();

    // merge the sublists together
    Barrier *b3 = new Barrier{3};

    thread([b3] {
        int half_list[LIST_SIZE/2];
        int totalcounter = 0;
        int counter1 = 0;
        int counter2 = 25;

        while(totalcounter < 50) {
            if (counter1 == 25) {
                half_list[totalcounter] = list[counter2];
                counter2++;
            }
            else if (counter2 == 50) {
                half_list[totalcounter] = list[counter1];
                counter1++;
            }
            else if (list[counter1] < list[counter2]) {
                half_list[totalcounter] = list[counter1];
                counter1++;
            }
            else {
                half_list[totalcounter] = list[counter2];
                counter2++;
            }

            totalcounter++;
        }

        for (int i = 0; i < LIST_SIZE/2; i++) {
            list[i] = half_list[i];
        }

        b3 -> sync();
    });

    thread([b3] {
        int half_list[LIST_SIZE/2];
        int totalcounter = 50;
        int counter1 = 50;
        int counter2 = 75;

        while(totalcounter < 100) {
            if (counter1 == 75) {
                half_list[totalcounter - 50] = list[counter2];
                counter2++;
            }
            else if (counter2 == 100) {
                half_list[totalcounter - 50] = list[counter1];
                counter1++;
            }
            else if (list[counter1] < list[counter2]) {
                half_list[totalcounter - 50] = list[counter1];
                counter1++;
            }
            else {
                half_list[totalcounter - 50] = list[counter2];
                counter2++;
            }

            totalcounter++;
        }

        for (int i = 0; i < LIST_SIZE/2; i++) {
            list[i + LIST_SIZE/2] = half_list[i];
        }

        b3 -> sync();
    });



    b3 -> sync();

    // merge first and second halves

    Barrier *b4 = new Barrier{2};

    thread([b4] {
        int duplicate[LIST_SIZE];
        int totalcounter = 0;
        int counter1 = 0;
        int counter2 = 50;

        while(totalcounter < 100) {
            if (counter1 == 50) {
                duplicate[totalcounter] = list[counter2];
                counter2++;
            }
            else if (counter2 == 100) {
                duplicate[totalcounter] = list[counter1];
                counter1++;
            }
            else if (list[counter1] < list[counter2]) {
                duplicate[totalcounter] = list[counter1];
                counter1++;
            }
            else {
                duplicate[totalcounter] = list[counter2];
                counter2++;
            }

            totalcounter++;
        }

        for (int i = 0; i < LIST_SIZE; i++) {
            list[i] = duplicate[i];
        }

        b4 -> sync();
    });

    b4 -> sync();


    for (int i = 0; i < LIST_SIZE; i++) {
        Debug::printf("*** %d\n", list[i]);
    }


    // initializes the array

}

