#include <stdio.h>

#include "src/go.h"

Value function_one() {
	printf("function_one\n");
	return asLong(1);
}

Value function_two() {
	printf("function_two\n");
	send(me(), asLong(2));
	return asLong(0xbad); // dont go here
}


int main() {
	Channel* channel_one = go(function_one);
	Value value_one = receive(channel_one);
	printf("%ld\n", value_one.asLong);

	Channel* channel_two = go(function_two);
	Value value_two = receive(channel_two);
	printf("%ld\n", value_two.asLong);

	return 0;
}
