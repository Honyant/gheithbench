// This test case implements a data structure in C++ to demonstrate the versatility of shared pointers.
// It is not thread safe! It is also not generic.
// Data structures that rely on arrays for the underlying implementation (dynamic arrays, stacks, hash maps, heaps) would be nice but strong pointers don't support operator[].
// Thus, I create a recursive data structure consisting of purely pointers and structs: a linked list, or queue.

// The test case is deceptively simple but presents a surprising edge case. Watch out!

#include "threads.h"
#include "shared.h"
#include "debug.h"
#include "heap.h"

int ct = 0;
int count() {
	return ct++;
}

constexpr int BUF_SIZE = 10000;
struct BigBuffer {
	int value = count();
	StrongPtr<BigBuffer> next;
	int buf[BUF_SIZE];
};

class Q {
public:
	StrongPtr<BigBuffer> head{nullptr};
	StrongPtr<BigBuffer> tail{nullptr};
	void add(StrongPtr<BigBuffer> b) {
		if (head == nullptr) {
			head = b;	
		}
		else {
			tail->next = b;
		}
		b->next = nullptr;
		tail = b;	
	}
	int remove() {
		if (head == nullptr) {
			return -1;
		}
		int v = head->value;
		head = head->next; // watch out!
		if (head == nullptr) {
			tail = nullptr;
		}
		return v;
	}
};

void kernelMain() {
	Q q{};
	for (int i = 0; i < 10; i++) {
		StrongPtr<BigBuffer> b{new BigBuffer};
		q.add(b);
	}
	for (int i = 0; i < 1000; i++) {
		q.remove();
		StrongPtr<BigBuffer> b{new BigBuffer};
		q.add(b);
	}
	for (int i = 0; i < 10; i++) {
		Debug::printf("*** %d\n", q.remove());
	}
}
