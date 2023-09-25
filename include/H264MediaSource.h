#pragma once
#include <unistd.h>
#include "SocketHelper.h"
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <Frame.h>

class H264MediaSource
{
public:
	H264MediaSource(std::string file_name);
	~H264MediaSource();

	/*	Read one frame from file then store it in frame
	*/
	int ReadFrame(Frame* frame);

private:
	std::string file_name_;
	FILE* fp_;

};