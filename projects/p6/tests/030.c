#include "src/go.h"
#include <stdio.h>

Channel *gchannels[10];
int i = -1;
int gcnter = 0;

void addGlobalChannel(Channel *ch) { gchannels[++i] = ch; }

void printTestCase(Value ret1, char *ans, char *c) {
  if (ret1.asString == ans) {
    printf("Passed %s\n", c);
  } else {
    printf("Wrong: %s\n", c);
  }
}

Value f5() {
  send(gchannels[i], asString("f5"));
  return asString("done");
}

Value f4() {
  go(f5);
  Value ret = receive(me());
  printTestCase(ret, "f5", "t1.3");
  gcnter++;
  if (gcnter == 1) {
    send(gchannels[i], asString("f5"));
  }
  return asString("f5");
}

void t1() {
  // 1. receive receive send send
  Channel *ch3 = go(f4);
  addGlobalChannel(ch3);

  Value ret3 = receive(ch3);
  gcnter++;
  if (gcnter == 1) { 
    send(gchannels[i], asString("f5"));
    printTestCase(ret3, "f5", "t1.3");
  } else if (gcnter == 2) {
    printTestCase(ret3, "f5", "t1.3");
  } else {
    printf("Got wrong value t1.3\n");
  }
}


Value adder() {
  long x = receive(me()).asLong;
  send(me(), asLong(x + 1));
  x = receive(me()).asLong;
  send(me(), asLong(x + 2));
  x = receive(me()).asLong;
  send(me(), asLong(x + 3));
  return asLong(1);
}

Value deadlockF1() {
  (void) receive(me());
  receive(me());
  return asString("deadlock");
}

void deadlock() {
  Channel* f1 =  go(deadlockF1);
  printf("Last line\n");
  send(f1, asString("never used"));
  (void) receive(f1);
  
}

int main() {
  //T1: Testing receive receive send send
  t1();

  //T2: Checking you can go back in memory 
  Channel *adderCh = go(adder);
  Value cur = asLong(0);
  long curVal = 0;
  for (int i = 1; i <= 3; i++) {
    curVal += i;
    send(adderCh, cur);
    cur = receive(adderCh);
    if (curVal != cur.asLong) {
      printf("Test 2: Wrong! curVal is %ld but yours is %ld\n", cur.asLong, curVal);
    } 
  }
  printf("Passed t2\n");
    long lastValueOfChannel = receive(adderCh).asLong;
    if (lastValueOfChannel == 1) {
      printf("Test 2: Got correct value!\n");
    }
    else {
      printf("Test 2: Wrong output. You got %ld\n", lastValueOfChannel);
    }
    
  //Test 3: deadlock!
  deadlock();
  printf("End. Should never run!\n");
}