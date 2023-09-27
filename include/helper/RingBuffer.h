#pragma once
#include <vector>
#include "helper/LOG.h"
#include <string_view>
#include <memory.h>
class RingBufferIterator;


class RingBuffer

{
	friend class RingBufferIterator;
public:
	RingBuffer(size_t capacity,std::string_view file_name );
	~RingBuffer();


	size_t getSize();

	RingBufferIterator begin();
	RingBufferIterator end();

	void SetOffect(const RingBufferIterator &iterator);

	

private:

	int ReadFile(int arr);

	FILE* fp_;
	size_t capacity_;
	size_t offect_;
	size_t remain_size_;
	int cur_arr_;
	std::vector<unsigned char> arr0_;
	std::vector<unsigned char> arr1_;
	
	bool eof_{ false };
	int eof_arr_{ -1 };

};

class RingBufferIterator
{
	friend class RingBuffer;
public:
	RingBufferIterator()=delete;
	~RingBufferIterator() {

	}

	RingBufferIterator(RingBuffer* ring_buffer, size_t offect, int cur_arr) :offect_(offect), cur_arr_(cur_arr), ring_buffer_(ring_buffer) {

	}
	RingBufferIterator(const RingBufferIterator& iterator):offect_(iterator.offect_), cur_arr_(iterator.cur_arr_), ring_buffer_(iterator.ring_buffer_) {

	}
	RingBufferIterator(RingBufferIterator&& iterator) noexcept :offect_(iterator.offect_), cur_arr_(iterator.cur_arr_), ring_buffer_(iterator.ring_buffer_) {
		iterator.cur_arr_ = -1;
		iterator.offect_ = -1;
		iterator.ring_buffer_ = nullptr;
	}

	unsigned char operator*() const{
		if (ring_buffer_ == nullptr) {
			LOG_INFO("ring_buffer_ is nullptr");
			return 0;
		}
		if (cur_arr_ == 0) {
			return ring_buffer_->arr0_[offect_];
		}
		else {
			return ring_buffer_->arr1_[offect_];
		}
	}

	// prefix
	RingBufferIterator& operator++();
	bool operator!=(const RingBufferIterator& iterator) const{
		return this->ring_buffer_ != iterator.ring_buffer_ || this->offect_ != iterator.offect_ || cur_arr_ != iterator.cur_arr_;
	}
	bool operator==(const RingBufferIterator& iterator) const{
		return this->ring_buffer_ == iterator.ring_buffer_ && this->offect_ == iterator.offect_ && cur_arr_ != iterator.cur_arr_;
	}
	RingBufferIterator& operator+=(size_t offect);

	friend RingBufferIterator operator+(const RingBufferIterator& iterator, size_t offect);

	size_t GetSizeBetween(const RingBufferIterator& iterator)const {
		if(iterator.cur_arr_==cur_arr_)
			return iterator.offect_ - offect_;
		else
			return iterator.offect_ + ring_buffer_->capacity_ - offect_;
	}

	std::vector<unsigned char> GetDataBetween(const RingBufferIterator& iterator)  const{
		if (iterator.cur_arr_ == cur_arr_) {
			std::vector<unsigned char> ret(iterator.offect_ - offect_);
			if (cur_arr_ == 0) {
				unsigned char* p = const_cast<unsigned char*>(ret.data());
				memcpy(p, &ring_buffer_->arr0_[offect_], iterator.offect_ - offect_);
			}
			else {
				unsigned char* p = const_cast<unsigned char*>(ret.data());
				memcpy(p, &ring_buffer_->arr1_[offect_], iterator.offect_ - offect_);
			}
			return ret;
		}
		else {
			std::vector<unsigned char> ret(iterator.offect_ + ring_buffer_->capacity_ - offect_);
			if (cur_arr_ == 0) {
				unsigned char* p = const_cast<unsigned char*>( ret.data());
				memcpy( p, &ring_buffer_->arr0_[offect_], ring_buffer_->capacity_-offect_);
				memcpy(p + ring_buffer_->capacity_ - offect_, &ring_buffer_->arr1_[0], iterator.offect_);
			}
			else {
				unsigned char* p = const_cast<unsigned char*>(ret.data());
				memcpy(p, &ring_buffer_->arr1_[offect_], ring_buffer_->capacity_-offect_);
				memcpy(p + ring_buffer_->capacity_ - offect_, &ring_buffer_->arr0_[0], iterator.offect_);
			}
			return ret;
		}
			
	}
private:
	size_t offect_;
	int cur_arr_;
	RingBuffer* ring_buffer_;
};

