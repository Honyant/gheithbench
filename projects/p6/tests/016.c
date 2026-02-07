#include <stdio.h>

#include "src/go.h"

//
// this test confirms that requests sent to a channel
// after that channel's coroutine has returned is
// that routine's return value
//
// that's all there is to it
//
// make sure to smash that bagel and like + subscribe
// for more fun C content
//

Value bagel_shop() {
  for (int bagels_sent = 1; bagels_sent < 10000; bagels_sent++) {
    send(me(), asInt(bagels_sent));
  }

  return asString("the bagel shop is closed, but you ate lots of bagels!");
}

Value poop_deli() {
  return asInt(0); // we are out... the police are cracking down
                   // because of that incident 3 doors down :-(
}

int main() {

  // TEST ONE: RECEIVE QUEUE RESOLVED
  // do you send the correct value before return?
  // do you send the return value after the coroutine returns?

  printf("welcome to the hunt for the holy bagel. your stomach quivers.\nyou "
         "decide to hit up the bagel shop.\n\n");

  Channel *bagel = go(bagel_shop);
  int chomp = 0;
  for (int bagels_eaten = 1; bagels_eaten < 10000; bagels_eaten++) {
    chomp = receive(bagel).asInt;
  }

  printf("%d bagels chomped! yummy!\n", chomp);
  char *final_string = receive(bagel).asString;
  printf("%s\n\n", final_string);

  // TEST TWO: REPEATED REQUEST TO CLOSED QUEUE
  // do you handle repeated receives once a c-routine returns?

  printf(
      "you are now on the hunt for that thing these kids on instagram reels "
      "are raving about.\nthey tell you to go to some sort of deli...\nsurely "
      "it's not really...\n\n");

  Channel *deli = go(poop_deli);
  for (int deli_runs = 1; deli_runs < 10000; deli_runs++) {
    if (receive(deli).asInt) {
      printf("For some reason, the deli gave you some supply even though it "
             "was their secret stash.\n");
      return 1;
    }
  }

  printf("the deli turned up nothing. apparently poop is now a schedule 1 "
         "substace.\nwho woulda thunk?\n\nanyway, you passed this test, so "
         "congrats.\n");

  return 0;
}
