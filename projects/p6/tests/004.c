#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "src/go.h"

Channel* globalChannel;
Channel* initChannel; // Channel to ensure ping runs first
int sum = 0;
bool terminate = false;
bool pingInitialized = false; // Flag to ensure ping runs first
uint64_t fibonacci(uint32_t n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

Value ping() {
    receive(initChannel); // Signal that ping has initialized
    pingInitialized = true;
    srand(420);
    while (1) {
        uint32_t fibIndex = rand() % 15;
        uint64_t fibValue = fibonacci(fibIndex);
        sum += fibValue;
        printf("Ping: Calculated Fibonacci(%u) = %" PRIu64 "\n", fibIndex, fibValue);
        
        send(globalChannel, asU64(fibValue)); // Send Fibonacci value
        
        if (terminate) {
            printf("Ping: Terminating because of termination signal from pong.\n");
            break;
        }
        if (rand() % 1000 == 0) { // Terminate with a small probability
            printf("Sum of Fibonacci values = %d\n", sum);
            printf("Ping: Terminating.\n");
            terminate = true;
            break;
        }
    }
    return asLong(0);
}

Value pong() {
    srand(420 + 1);
    if (!pingInitialized) {
        send(initChannel, asLong(0)); // Signal that ping has initialized
    }
    while (1) {
        receive(globalChannel); // Wait for Fibonacci value
        uint32_t fibIndex = rand() % 15;
        uint64_t fibValue = fibonacci(fibIndex);
        sum += fibValue;
        printf("Pong: Received Fibonacci value, calculated own Fibonacci(%u) = %" PRIu64 "\n", fibIndex, fibValue);
        if (terminate) {
            printf("Pong: Terminating because of termination signal from ping.\n");
            break;
        }
        if (rand() % 1000 == 0) { // Terminate with a small probability
            printf("Sum of Fibonacci values = %d\n", sum);
            printf("Pong: Terminating.\n");
            terminate = true;
            break;
        }
    }
    return asLong(0);
}

int main() {
    Channel* ch = channel();
    globalChannel = channel();
    initChannel = channel(); // Initialize channel to coordinate ping initialization
    go(ping);
    go(pong);
    printf("Main function completed. Coroutines are running...\n");
    receive(ch);
    return 0;
}
