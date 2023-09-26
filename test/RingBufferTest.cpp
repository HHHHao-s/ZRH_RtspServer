#include "RingBuffer.h"
#include <iostream>
#include <stdio.h>

int main() {

	RingBuffer ring_buffer(1024,ROOT_DIR "/test/test.txt");

	RingBufferIterator iterator = ring_buffer.begin();

	while (iterator != ring_buffer.end()) {
		printf("%c", *iterator);
		++iterator;
	}
	
	return 0;
}