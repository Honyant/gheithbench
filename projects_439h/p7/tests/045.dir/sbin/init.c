#include "debug.h"
#include "libc.h"
int main(int argc, char **argv) {

  int failed_chdir = chdir("/nonexistentdir/");

  ASSERT(failed_chdir == -1);

  printf("*** passed test for nonexistent chdir.\n");

  int passed_chdir = chdir("/manifesto");

  ASSERT(passed_chdir == 0);

  // let's do a simple read-from-file test.

  int education = open("education.txt", 0);

  cp(education, 1);

  // okay, now for a simple pre-emption + fork test. we're going to spawn
  // threads that spin, (including this one), and one that actually prints what
  // we want.
  int id;
  int pastIDs[3];
  for (int i = 0; i < 3; i++) {
    id = fork(); // keep in mind that this growth is exponential for each loop
    pastIDs[i] = id;
  }

  // this part is pretty tricky, but essentially I'm trying to spawn a
  // reasonable number of threads that will spin forever, so that I can force
  // the kernel to pre-empt them. this just ensures that you still have
  // pre-emption on and that it works.

  // along the way, I also test wait() and fork().

  if (!pastIDs[0] && !pastIDs[1] && !pastIDs[2]) {
    // we are the child from every fork();

    printf("*** made it to our final process! pre-emption probably works\n");

    id = fork();
    uint32_t status = 42;
    if (id != 0) {
      wait(id, &status);
      ASSERT(status == 800813);
      printf("*** successfully waited on child!\n");
      printf("*** time to shut down so we don't spin forever\n");
      shutdown();
    } else {
      // add some artificial delay with a very basic sanity check
      const uint32_t N = 10000;
      char *mmaped = naive_mmap(10, 0, 0, 0);
      for (int i = 0; i < N; i++) {
        mmaped[i % 2] = 'c';
      }
      // now return so the parent can wake up
      return 800813;
    }
  } else {
    // we are one of the many other processes. time to spin forever.
    char *mmaped = naive_mmap(10, 0, 0, 0);
    int i = 0;
    while (1) {
      i++;
      mmaped[i % 2] = 'c';
    }
  }

  printf("*** how did we get here?\n");
  shutdown();
  return 0;
}
