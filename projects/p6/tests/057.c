#include <stdio.h>

#include "src/go.h"

Value functionA() {
    send(me(), asInt(55));
	return asLong(1);
}

Value functionB() {
	send(me(), asU64(99));
	return asU64(105);
}

Value functionC() {
    for (uint64_t i = 0; i < 10; i++) {
        send(me(), asU64(i));
    }
	return asU64(11);
}


int main() {
	Channel* c1 = go(functionA);
	Value val1 = receive(c1);
	printf("%d\n", val1.asInt);

	Channel* c2 = go(functionB);
	Value val2 = receive(c2);
	printf("%d\n", val2.asInt);

    Channel* c3 = go(functionB);
    for(int j = 0; j < 20; j ++) {
        uint64_t x = receive(c3).asU64;
        printf("%ld\n", x);
    }

	return 0;
}