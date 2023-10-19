#include "live/AACMediaSource.h"




AACMediaSource::AACMediaSource(RtspContext* ctx, std::string_view file_name):MediaSource(ctx, file_name,AAC_FRAME_SIZE_FPS), ring_buffer_(MAX_FRAME_SIZE, file_name) {

	for (size_t i = 0; i < max_frame_size_; ++i) {
		CacheFrame();
	}

}
AACMediaSource::~AACMediaSource() {

}

	/*	Read one frame from file then store it in frame
	*/
std::vector<unsigned char> AACMediaSource::ReadFrame() {

    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]() {
        //LOG_INFO("%d", (int)frames_.size());
        return !frames_.empty();
        });
    std::vector<unsigned char> ret = std::move(frames_.front());
    frames_.pop();
    lock.unlock();
    CacheFrame();
    return ret;

}
std::vector<unsigned char> AACMediaSource::GetFrameFromFile() {

       AdtsHeader adts_header;
       std::vector<unsigned char> frame;
       auto it = ring_buffer_.begin();
       if (parseAdtsHeader(it.get(), &adts_header) == -1) {
		   LOG_INFO("parseAdtsHeader(&(*it), &adts_header) == -1");
		   return frame;
	   }
       frame.resize(adts_header.aacFrameLength-7);
	   memcpy(frame.data(), (it+7).get(), adts_header.aacFrameLength - 7);
       
	   ring_buffer_.SetOffect(it+ adts_header.aacFrameLength);
	   return frame;


}




void AACMediaSource::CacheFrame() {
    ctx_->thread_pool_->enqueue([](AACMediaSource* t) {
        std::unique_lock<std::mutex> lock(t->mutex_);

        auto frame = t->GetFrameFromFile();

        if (frame.size() > 0) {
            t->frames_.emplace(std::move(frame));
        }
        else {
            //LOG_INFO("frame.size() == 0");
        }
        lock.unlock();
        // notify that the cache is not empty
        t->cv_.notify_all();
        }, this);
}




