#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter = 3;
static uint32_t counter2 = 0;
static uint32_t sum = 0;

// Triangular numbers count the number of objects in an equilateral triangle.
// The nth triangular number can be calculated by finding the sum of all natural
// numbers up to n, or using recursion.


// calculates triangular numbers
// tests that critical allows recursive function calls to pass through
int triangle(int n) {
    if (n <= 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }
    if (n == 2) {
        return 3;
    }
    return 3*triangle(n - 1) - 3*triangle(n - 2) + triangle(n - 3); 
}

// draws equilateral triangle
void drawTriangle(int height) {
    char drawing[100];
    int index = 0;

    for (int i = 1; i <= height; i++) {
        for (int s = 0; s < 3; s++) {
            drawing[index++] = '*';
        }
        drawing[index++] = ' ';
        for (int j = 1; j <= height - i; j++) {
            drawing[index++] = ' ';
        }
        for (int k = 1; k <= i; k++) {
            drawing[index++] = '*';
            if (k < i) {
                drawing[index++] = ' ';
            }
        }
        drawing[index++] = '\n';
    }
    drawing[index] = '\0';

    Debug::printf("%s",drawing);
}

// tests that critical allows nested function calls to pass through
void work1() {
    int trgl = triangle(counter);
    Debug::printf("*** triangle %d: %d\n",counter,trgl);
    drawTriangle(counter);

    sum += trgl;
    counter ++;
}

// tests that all work1() calls were finished before the last work2() is called
void work2() {
    counter2++;
    if (counter2 == 4) {
        if (counter != 7) {
            Debug::printf("*** error\n");
        }
        Debug::printf("*** sum: %d\n",sum);
    }
}

void kernelMain(void) {
    critical(work1);
    critical(work2);
}
