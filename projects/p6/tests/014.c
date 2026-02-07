//Comment vers: https://tinyurl.com/Test014
#include "src/go.h"
#include <stdio.h>
#include <stdlib.h>

Channel *ch1, *ch2, *ch3, *ch4;
uint64_t c1, c2, c3, c4;
int done1, done2, done3, done4;
#define HP1 1
#define HP2 100
#define HP3 10000
#define HP4 1000000

//Let's play whack-a-mole
Value whack()
{
	if (done1 && done2 && done3 && done4)
		return asLong(0);
	int r = rand() % 4;
	if (r == 0)
	{
		if (c1 < HP1)
		{
			c1 += 1;
			send(ch1, asLong(0));
		}
		else
			send(ch1, asLong(1));
	}
	else if (r == 1)
	{
		if (c2 < HP2)
		{
			c2 += 1;
			send(ch2, asLong(0));
		}
		else
			send(ch2, asLong(1));
	}
	else if (r == 2)
	{
		if (c3 < HP3)
		{
			c3 += 1;
			send(ch3, asLong(0));
		}
		else
			send(ch3, asLong(1));
	}
	else if (r == 3)
	{
		if (c4 < HP4)
		{
			c4 += 1;
			send(ch4, asLong(0));
		}
		else
			send(ch4, asLong(1));
	}
	again();
	return asLong(-1);
}

Value ch1Receiver()
{
	if (receive(ch1).asLong && !done1)
	{
		printf("Mole 1 has been whacked %li times and is dead\n", c1);
		done1 = 1;
	}
	again();
	return asLong(-1);
}

Value ch2Receiver()
{
	if (receive(ch2).asLong && !done2)
	{
		printf("Mole 2 has been whacked %li times and is dead\n", c2);
		done2 = 1;
	}
	again();
	return asLong(-1);
}

Value ch3Receiver()
{
	if (receive(ch3).asLong && !done3)
	{
		printf("Mole 3 has been whacked %li times and is dead\n", c3);
		done3 = 1;
	}
	again();
	return asLong(-1);
}

Value ch4Receiver()
{
	if (receive(ch4).asLong && !done4)
	{
		printf("Mole 4 has been whacked %li times and is dead\n", c4);
		done4 = 1;
	}
	again();
	return asLong(-1);
}

int main()
{
	c1 = c2 = c3 = c4 = 0;
	done1 = done2 = done3 = done4 = 0;
	ch1 = channel();
	ch2 = channel();
	ch3 = channel();
	ch4 = channel();
	go(whack);
	go(whack);
	go(ch1Receiver);
	go(ch2Receiver);
	go(ch3Receiver);
	go(ch4Receiver);
	while (true)
	{
		if (done1 && done2 && done3 && done4)
		{
			printf("Congratulations! You killed all 4 moles, you monster.");
			return 0;
		}
		else
		{
			send(ch1, asLong(0));
			send(ch2, asLong(0));
			send(ch3, asLong(0));
			send(ch4, asLong(0));
		}
	}
}