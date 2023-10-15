#pragma once
#include <string>
#include <stdint.h>
#include <memory>
#include "helper/RtspContext.h"
#include "live/MediaSource.h"
#include "live/Rtp.h"
#include "helper/Event.h" 
enum PacketType
{
    UNKNOWN = -1,
    RTPPACKET = 0,
};
class Sink
{
public:
    

    typedef void (*SessionSendPacketCallback)(void* arg1, void* arg2, void* packet, PacketType packetType);

    Sink(RtspContext* ctx, std::unique_ptr<MediaSource> media_source, int payloadType) :ctx_(ctx), media_source_(std::move(media_source)), buf(RTP_MAX_PKT_SIZE + sizeof(RtpPacket) + 2)
    {


        timer_id_ = ctx_->scheduler_->StartTimer(cbTimeout, this, 1000 / media_source_->GetFPS());
        
        rtp_header_.csrcLen = 0;
        rtp_header_.extension = 0;
        rtp_header_.padding = 0;
        rtp_header_.version = 2;
        rtp_header_.payloadType = payloadType;
        rtp_header_.marker = 0;
        rtp_header_.seq = 0;
        rtp_header_.timestamp = 0;
        rtp_header_.ssrc = rand();

	}
    virtual ~Sink() {
        LOG_INFO("Sink::~Sink()");
    }

    void stopTimerEvent();


    virtual std::string getMediaDescription(uint16_t port) = 0;
    virtual std::string getAttribute() = 0;

    void setSessionCb(SessionSendPacketCallback cb, void* arg1, void* arg2) {
        session_send_packet_ = cb;
		session_ptr_ = arg1;
		mArg2 = arg2;
    }

protected:

    virtual void sendFrame(std::vector<unsigned char> frame) = 0;
    void sendRtpPacket(RtpPacket* packet) {
       
        session_send_packet_(session_ptr_, mArg2, packet, PacketType::RTPPACKET);
    }

    
private:

    static void cbTimeout(void* arg) {
        Sink* sink = static_cast<Sink*>(arg);
		sink->handleTimeout();
    }
    void handleTimeout() {
        std::vector<unsigned char> frame = media_source_->ReadFrame();
        if (frame.size() == 0) {
			return;
		}
		sendFrame(frame);
    }

protected:
    RtspContext* ctx_;
    timer_id_t timer_id_;


    std::unique_ptr<MediaSource> media_source_;
    SessionSendPacketCallback session_send_packet_;
    void* session_ptr_;
    void* mArg2;

    RtpHeader rtp_header_;
    std::vector<unsigned char> buf;

    
};