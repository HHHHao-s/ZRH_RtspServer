#pragma once
#include <unistd.h>
#include "helper/SocketHelper.h"
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <string_view>
#include <mutex>
#include <list>
#include "helper/RtspContext.h"
#include "helper/RingBuffer.h"
#include "live/MediaSource.h"
#define MAX_FRAME_SIZE 1024 * 1024
#define H264_FRAME_FPS 30
class H264MediaSource;

//void CacheTask(H264MediaSource* t);

class H264MediaSource :public MediaSource
{
	//friend void CacheTask(H264MediaSource* t);
public:
	H264MediaSource(RtspContext * ctx, std::string_view file_name);
	~H264MediaSource();

	/*	Read one frame from file then store it in frame
	*/
	std::vector<unsigned char> ReadFrame();
	std::vector<unsigned char> GetFrameFromFile();

private:

	
	void CacheFrame();


	RingBuffer ring_buffer_;
	enum 
	{
		START_CODE3,
		START_CODE4
	}start_code_;

	std::queue<std::vector<unsigned char>> frames_;
	size_t max_frame_size_{ 8 };
	

};