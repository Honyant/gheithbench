#include "walmart_omp.h"
#include "fixed.h"

static fixed integrand(fixed x) {
	fixed sq, n, d;
	sq = x * x;
	n = x - sq;
	n = n * n;
	d = sq + 1;
	n = n * n;
	return n / d;
}

static void add(fixed* sum, fixed summand) {
	*sum += summand;
}

static fixed pi() {
	using namespace WOMP;
	fixed* sum = (fixed*) naive_mmap(sizeof(fixed), true, {}, 0);
	fixed h = fixed::ticks(64);
	/* this code is slow because every iteration requires two syscalls to get a critical section
	int id = get_critical_id();
	auto add_integrand = [sum, id] (fixed x) {
		fixed y = integrand(x);
		critical([sum, y] {
			*sum += y;
		}, id);
	};
	parallel_for<fixed>(add_integrand, 0, h, 1, 64);
	*/

	// the below functionality is faster
	parallel_for_reduction<fixed,fixed>(integrand, add, 0, sum, 0, h, 1, 64);
	
	*sum -= (integrand(0) + integrand(1)) / 2; // trapezoid rule
	fixed output = fixed(22) / 7 - h * (*sum);
	naive_unmap(sum); // this does nothing because sum was in shared space
	return output;	
}

int main() {
	pi().print();
	shutdown();
	return -1;
}
