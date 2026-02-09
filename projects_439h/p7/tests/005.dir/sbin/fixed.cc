#include <cstdio>
#include <cstdint>
#include "fixed.h"

// Testing for the fixed point class.
int main() {
	(fixed(0)).print();
	(fixed(22) / 7).print();
	fixed h = fixed::ticks(64);
	fixed sum = fixed::ticks(2000);
	(fixed(22) / 7 - h * sum).print();
	return 0;
}
