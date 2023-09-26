#pragma once
#include <unistd.h>
#include "SocketHelper.h"
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <Frame.h>
#include <vector>
#include <string_view>
#include "RingBuffer.h"

constexpr size_t MAX_BUFFER_SIZE = 1024 * 1024;

class H264MediaSource
{
public:
	H264MediaSource(std::string_view file_name);
	~H264MediaSource();

	/*	Read one frame from file then store it in frame
	*/
	std::vector<unsigned char> ReadFrame();

private:
	std::string_view file_name_;
	RingBuffer ring_buffer_;
	enum 
	{
		START_CODE3,
		START_CODE4
	}start_code_;

};