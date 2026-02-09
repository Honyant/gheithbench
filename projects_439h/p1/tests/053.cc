#include "debug.h"
#include "critical.h"
#include "atomic.h"

// Towers of Hanoi recursive function: 
// Reference: https://www.geeksforgeeks.org/c-program-for-tower-of-hanoi/#


void hanoi(int n, char from, char to, char aux){
    if(n == 1){
        Debug::printf("***Move disk 1 from %c to %c\n", from, to);
        return;
    }
    hanoi(n-1, from, aux, to);
    Debug::printf("***Move disk %d from %c to %c\n", n, from, to);
    hanoi(n-1, aux, to, from);
}

void work() {
    int numDisks = 5;
    hanoi(numDisks, 'A', 'C', 'B');
}

void F(void){
    critical(work);
}

void E(void){
    critical(F);
}

void D(void){
    critical(E);
}

void C(void){
    critical(D);
}

void B(void){
    critical(C);
}

void A(void){
    critical(B);
}

void kernelMain(void) {
    critical(A);
    critical(A);
}