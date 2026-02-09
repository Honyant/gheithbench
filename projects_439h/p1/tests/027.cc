#include "debug.h"
#include "critical.h"
#include "atomic.h"


static uint32_t counter = 0;

static uint32_t interleaving_counter = 1;

Atomic<bool> critical_flag { false };
uint32_t critical_depth = 0;
uint32_t critical_owner = 439;

static Barrier* ending = new Barrier(kConfig.totalProcs);


/*
    TEST ONE - simple test of critical section and basic synchronization...
    if you pass t0 and t1, you should pass this
*/
void test_one_simple_synchronization(){
    Debug::printf("*** test one - simple synch iterations: %d\n", counter);
    counter++;
}

/*
    TEST TWO - just simple math test to see that the order is properly
    maintained and that your critical section is working as expected

    careful to avoid division by 0!
*/
void test_two_math(){
    int a = 439;
    int b = 0;     
    int c = 429;
    b = c;
    a = a - c; // now, a=10, b = 429, and c = 429
    
    
    int result = (c / b) + a + c - 1;   //careful that you don't div by 0!
    Debug::printf("*** CONGRATS ON MAKING IT THROUGH THE FIRST WEEK OF  %dH, FALL 2024!!\n", result); 
    
}


/*
    TEST THREE - testing edge cases with nested critical sections and conflicitng variable names
*/
void test_three_edge(){
    critical([]() {
            Debug::printf("*** critical section layer 2! 'critical depth' (but not from critical.cc!): %d\n", critical_depth);
            critical_depth++;
        });
}

void sequence(){
    test_one_simple_synchronization();
    test_two_math();
    test_three_edge();
    interleaving_counter -= 1;
}

void sequence_two(){
    interleaving_counter += 1;
}


/* Called by all cores */
void kernelMain(void) {
//atomic,h
    
    critical(sequence);
    sequence_two(); 
    ending->sync(); // barrier to prevent the next line from printing until the end
    Debug::printf("*** since we went into and out of the critical section, this count should be 1, and this message will print at the end x4 due to the barrier. Your count: %d\n", interleaving_counter);
        
}



