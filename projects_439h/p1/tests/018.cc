#include "critical.h"
#include "debug.h"

/*
    HOW SH*TTY ARE BAGELS ON GUAD?

    This test case will make ready use of the critical section to...
    calculate the variance of the Yelp reviews (in the last 2 years) for
    Einstein Bros. Bagels @ 2404 Guadalupe St.

    This test ensures that (in your implementation):

    1. Static variables are updated correctly when run in the critical section,
    2. Critical section recursion is handled
    3. Lock works properly and consistently when there are contests
 */

// All the reviews on Yelp since 2022
// 1-5 star scale
static const int reviews[] = {1, 5, 5, 4, 1, 1, 4, 4, 1, 1,
                              2, 3, 4, 2, 1, 1, 1, 5, 5, 1};
static const int REVIEW_LEN = 20;

// Here are the calculation variables modified by work
static int sum = 0;
static int sq_sum = 0;
static int curr_review = 0;
static bool done = false;
static double var = 0;

// I'm lazy, but C++20 :3
static auto square = []<typename T>(T x) { return x * x; };

void find_var() {
  // Skip recalculation
  if (done) {
    return;
  }
  // Check if work is done
  else if (curr_review == REVIEW_LEN) {
    // sum(x^2) - sum(x)^2
    var = ((double)sq_sum / REVIEW_LEN) - square((double)sum / REVIEW_LEN);
    Debug::printf("*** %f\n", var);
    done = true;
    return;
  }

  // Calculate sums for current index
  sum += reviews[curr_review];
  sq_sum += (square(reviews[curr_review]));
  curr_review += 1;

  // Recurse all the way down
  // Only 1 core will do the work
  critical(find_var);

  /*
    Strictly speaking, the `critical` call is not necessary here, but it
    will make sure that you handle the same core trying to enter the
    critical section.
  */
}

/* Called by all cores */
void kernelMain(void) {
  // This way, there will be (cores * loop_iters) - 1 contests
  // So, 99 contests
  for (int i = 0; i < 25; ++i) {
    // The C way, use function pointers
    critical(find_var);
  }
}
