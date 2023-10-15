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



constexpr size_t MAX_BUFFER_SIZE = 1024 * 1024;
class MediaSource;

//void CacheTask(H264MediaSource* t);

class MediaSource
{
	//friend void CacheTask(H264MediaSource* t);
public:
	MediaSource(RtspContext * ctx, std::string_view file_name, int fps) :ctx_(ctx), file_name_(file_name), FPS_(fps) {
	}
	virtual ~MediaSource() {}

	/*	Interface
	*/
	virtual std::vector<unsigned char> ReadFrame() = 0;
	int GetFPS() {
		return FPS_;
	}

protected:

	std::mutex mutex_;
	std::condition_variable cv_;
	RtspContext * ctx_;
	std::string_view file_name_;
	
	int FPS_;


};