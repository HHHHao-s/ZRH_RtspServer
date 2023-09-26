#include "H264MediaSource.h"
#include <memory>
#include "LOG.h"

static inline int startCode3(const RingBufferIterator &it)
{
    if (*it == 0 && *(it+1) == 0 && *(it + 2) == 1)
        return 1;
    else
        return 0;
}

static inline int startCode4(const RingBufferIterator& it)
{
    if (*it == 0 && *(it + 1) == 0 && *(it + 2) == 0 && *(it+3)==1)
        return 1;
    else
        return 0;
}



H264MediaSource::H264MediaSource(std::string_view file_name) :file_name_(file_name), ring_buffer_(MAX_BUFFER_SIZE, file_name)
{
   
    
        
}

H264MediaSource::~H264MediaSource()
{
	
}

std::vector<unsigned char> H264MediaSource::ReadFrame() {
	
    auto it = ring_buffer_.begin();
    

    if (it == ring_buffer_.end()) {
		LOG_INFO("it == ring_buffer_.end()");
		return std::vector<unsigned char>();
	}

    
    if (startCode3(it)||  startCode4(it)) {
        // find next start code
        it += 3;
        while (it != ring_buffer_.end())
        {
            
            if (startCode3(it) || startCode4(it)) {
                break;
            }
            ++it;

        }
        if (it == ring_buffer_.end()) {
			LOG_INFO("it == ring_buffer_.end()");
			return std::vector<unsigned char>();
		}
        else {

			auto it2 = ring_buffer_.begin();

			auto ret = it2.GetDataBetween(it);
            ring_buffer_.SetOffect(it);
            return ret;
		}
	}
	else {
		return std::vector<unsigned char>();
	}
		
    
}


