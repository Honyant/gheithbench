#include "debug.h"
#include "critical.h"
#include "atomic.h"

// Mersenne Primes
static uint32_t mersenneNumber = 0;
bool isPrime = false;

void ifPrime() {
    if (mersenneNumber <= 1) {
        isPrime = false;
        return;
    }
    // Find sqrt of number 
    uint64_t left = 2;
    uint64_t right = mersenneNumber/2;
    while (left <= right) {
        uint64_t middle = left + (right - left)/2;
        uint64_t sq = (long) middle*middle;
        if (sq == mersenneNumber) {
            right = middle;
            break; 
        } 
        if (sq < mersenneNumber && (middle+1)*(middle+1) > mersenneNumber) {
            right = middle;
            break;
        }
        if (sq < mersenneNumber) left = middle+1;
        else right = middle-1;
    }
    // Checks if prime
    uint64_t sqrt = right;
    for (uint32_t i = 2; i <= sqrt; i++) {
        if (mersenneNumber % i == 0) {
            isPrime = false;
            return;
        }
    }
    isPrime = true;
    return;
}

void calc() {
    for (int i=0; i<=31; i++) {
        mersenneNumber = 1 << i;
        mersenneNumber -= 1;
        ifPrime();
        if (isPrime) Debug::printf("*** Mersenne Prime Number: %d\n", mersenneNumber);
    }
}

/* Called by all cores */
void kernelMain(void) {
    // Checks the first 30 mersenne numbers if they are prime (2^n - 1)
    critical(calc);
}
