#include "helper/ThreadPool.h"

#include <iostream>


int main() {

	ThreadPool t(4);
	for (int i = 0; i < 10; i++) {
		t.enqueue([]() { std::cout << "Hello "; });
	}
	

	return 0;
}