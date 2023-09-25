#include "H264MediaSource.h"
#include <memory>
#include "LOG.h"

static inline int startCode3(char* buf)
{
    if (buf[0] == 0 && buf[1] == 0 && buf[2] == 1)
        return 1;
    else
        return 0;
}

static inline int startCode4(char* buf)
{
    if (buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1)
        return 1;
    else
        return 0;
}

static char* findNextStartCode(char* buf, int len)
{
    int i;

    if (len < 3)
        return NULL;

    for (i = 0; i < len - 3; ++i)
    {
        if (startCode3(buf) || startCode4(buf))
            return buf;

        ++buf;
    }

    if (startCode3(buf))
        return buf;

    return NULL;
}

H264MediaSource::H264MediaSource(std::string file_name) :file_name_(file_name)
{
	this->fp_ = fopen(file_name.c_str(), "rb");
	if (this->fp_ == NULL) {
		LOG_ERROR("Open file %s error\n", file_name.c_str());
	}
    LOG_INFO("open success");
}

H264MediaSource::~H264MediaSource()
{
	if (this->fp_ != NULL) {
		if (fclose(this->fp_) < 0) {
			LOG_ERROR("Close file %s error\n", file_name_.c_str());
		}
	}
}

int H264MediaSource::ReadFrame(Frame* frame) {
	
    char* buf = frame->data;
	
	size_t n = fread(buf, 1, 1024*1024, this->fp_);

    LOG_INFO("read size %d", (int)n);
    
    if (n < 0) {
        LOG_ERROR("read file error");
		return -1;
    }
    else if (n == 0) {
		LOG_INFO("read file end");
        return -1;
    }

    if (!startCode3(buf) && !startCode4(buf))
    {
		LOG_INFO("not start with 0x000001 or 0x00000001");
		return -1;
	}  

    char* next_frame_begin = findNextStartCode(buf+3, n-3);
    size_t frame_size = next_frame_begin - buf;
    fseek(this->fp_, frame_size-n, SEEK_CUR);

    

    frame->size = frame_size;

    return 0;
}


