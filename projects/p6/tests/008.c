#include <stdio.h>
#include "src/go.h"

// Tests basic functionality:
// Sending and receiving from channels (including context switching, of course)
// Calling go (the channel returned must be usable and unique)
// Returning from a coroutine
// Exiting when all coroutines are blocked
// Calling me
// Calling again

int again_count = 0;

Value f1() {
	printf("first\n");
	send(me(), asLong(1));
	return asLong(111);
}

Value f2() {
	printf("second\n");
	return asLong(222);
}

Value f3() {
	printf("third\n");
	again_count++;
	if (again_count <= 1) {
		again();
	}
	Channel* deadend = channel();
	send(deadend, asLong(0));
	return asLong(333);
}

int main() {
	Channel* ch1 = go(f1);
	Value v1 = receive(ch1);
	printf("%ld\n", v1.asLong);

	Channel* ch2 = go(f2);
	Value v2 = receive(ch2);
	printf("%ld\n", v2.asLong);
	v2 = receive(ch2);
	printf("%ld\n", v2.asLong);

	Channel* ch3 = go(f3);
	Value v3 = receive(ch3); // block everything
	printf("%ld\n", v3.asLong);
	return 0;
}
