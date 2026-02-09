#include "sys.hpp"
#include "libc.hpp"

int main() {
	execl("/sbin/pi", "pi", 0);
	return 0;
}
