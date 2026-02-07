#include <stdio.h>
#include "src/go.h"

Value f1() {
	send(me(), asLong(0));
	return asLong(10);
}

Value f2() {
	return asLong(20);
}


int main() {
    //testing fpr basic send amd receive
	Channel* ch1 = go(f1);
	Value out1 = receive(ch1);
	printf("%ld\n", out1.asLong);

    //testing for return value setting
	Channel* ch2 = go(f2);
	Value out2 = receive(ch2);
	printf("%ld\n", out2.asLong);
	
    out2 = receive(ch2);
	printf("%ld\n", out2.asLong);

	return 0;
}