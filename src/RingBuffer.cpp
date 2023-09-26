#include "RingBuffer.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


RingBuffer::RingBuffer(size_t capacity, std::string_view file_name ) :capacity_(capacity), offect_(0), remain_size_(-1), cur_arr_(0), arr0_(capacity), arr1_(capacity) {
	fp_ = fopen(file_name.data(), "rb");
	if (fp_ < 0) {
		LOG_ERROR("open file error");
	}

	if (ReadFile(0) == -1) {
		LOG_ERROR("read file error");
	}
	

}

int RingBuffer::ReadFile(int arr) {
	if (fp_ == NULL) {
		LOG_ERROR("fp_ is nullptr");
		return -1;
	}
	if (eof_) {
		LOG_INFO("eof_ is true");
		return -1;
	}
	if (arr == 0) {
		size_t read_size = fread(arr0_.data(), 1, capacity_, fp_);
		if (read_size < 0) {
			LOG_ERROR("read file error");
			return -1;
		}
		if (read_size < capacity_) {
			eof_ = true;
			eof_arr_ = arr;
			remain_size_ = read_size;
		}		
	}
	else {
		size_t read_size = fread(arr1_.data(), 1, capacity_, fp_);
		if (read_size < 0) {
			LOG_ERROR("read file error");
			return -1;
		}
		if (read_size < capacity_) {
			eof_ = true;
			eof_arr_ = arr;
			remain_size_ = read_size;
		}
	}
	return 0;
}


RingBuffer::~RingBuffer() {
	if (fp_ != NULL) {
		if (fclose(fp_)<0) {
			LOG_ERROR("close file error");
		}
	}
}

size_t RingBuffer::getSize() {
	return capacity_;
}

RingBufferIterator RingBuffer::begin() {
	return RingBufferIterator(this, offect_, cur_arr_);
}

RingBufferIterator RingBuffer::end() {
	if (eof_) {
		return RingBufferIterator(this, remain_size_, eof_arr_);
	}
	else {
		return RingBufferIterator(this, capacity_+1, cur_arr_);// iterator will not reach the end until it read the last byte
	}
	
}

void RingBuffer::SetOffect(const RingBufferIterator& iterator) {
	offect_ = iterator.offect_;
	cur_arr_ = iterator.cur_arr_;
}

RingBufferIterator& RingBufferIterator::operator++() {
	if (ring_buffer_ == nullptr) {
		LOG_INFO("ring_buffer_ is nullptr");
		return *this;
	}
	if (offect_ == ring_buffer_->capacity_ - 1) {
		offect_ = 0;
		cur_arr_ = (cur_arr_ + 1) % 2;
		ring_buffer_->ReadFile(cur_arr_);
	}
	else {
		offect_++;
	}
	return *this;
}

RingBufferIterator& RingBufferIterator::operator+=(size_t offect) {
	if (ring_buffer_ == nullptr) {
		LOG_INFO("ring_buffer_ is nullptr");
		return *this;
	}
	if (offect_ + offect < ring_buffer_->capacity_) {
		offect_ += offect;
	}
	else {
		if (offect > 2 * ring_buffer_->capacity_ - offect_) {
			LOG_INFO("offect is too big");
			return *this;
		}


		offect_ = offect_ + offect - ring_buffer_->capacity_;
		cur_arr_ = (cur_arr_ + 1) % 2;
		ring_buffer_->ReadFile(cur_arr_);
	}
	return *this;
}

RingBufferIterator operator+(const RingBufferIterator& iterator, size_t offect) {
	RingBufferIterator temp(iterator);
	temp += offect;
	return temp;
}

