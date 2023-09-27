#include "live/H264MediaSource.h"
#include <memory>
#include "helper/LOG.h"
#include "helper/ThreadPool.h"

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



H264MediaSource::H264MediaSource(std::shared_ptr<RtspContext> ctx, std::string_view file_name) :ctx_(ctx), file_name_(file_name), ring_buffer_(MAX_BUFFER_SIZE, file_name)
{
    for (size_t i = 0; i < max_frame_size_; ++i) {
	   CacheFrame();
   }
    
        
}

H264MediaSource::~H264MediaSource()
{
	
}


// Read one frame from file return
// concurrent call is not allowed
std::vector<unsigned char> H264MediaSource::GetFrameFromFile() {

    

    auto it = ring_buffer_.begin();


    if (it == ring_buffer_.end()) {
        LOG_INFO("it == ring_buffer_.end()");
        return  std::vector<unsigned char>() ;
    }


    if (startCode3(it) || startCode4(it)) {
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

std::vector<unsigned char> H264MediaSource::ReadFrame() {
	
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]() {return !frames_.empty(); });
    std::vector<unsigned char> ret = std::move(frames_.front());
    frames_.pop();
    lock.unlock();
    CacheFrame();
    return ret;
		
    
}

//void CacheTask(H264MediaSource *t) {
//    std::unique_lock<std::mutex> lock(t->mutex_);
//
//    auto frame = t->GetFrameFromFile();
//
//    if (frame.size() > 0) {
//        t->frames_.push(frame);
//    }
//    else {
//        LOG_INFO("frame.size() == 0");
//    }
//    lock.release();
//    // notify that the cache is not empty
//    t->cv_.notify_all();
//}

void H264MediaSource::CacheFrame() {

    
    ctx_->thread_pool_->enqueue([](H264MediaSource* t) {
        std::unique_lock<std::mutex> lock(t->mutex_);

        auto frame = t->GetFrameFromFile();

        if (frame.size() > 0) {
            t->frames_.push(frame);
        }
        else {
            LOG_INFO("frame.size() == 0");
        }
        lock.unlock();
        // notify that the cache is not empty
        t->cv_.notify_all();
     }, this);

}
