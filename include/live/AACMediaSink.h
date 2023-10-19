#pragma once
#include "AACMediaSource.h"
#include "live/Sink.h"
#include <string>

#define RTP_PAYLOAD_TYPE_AAC 97
class AACMediaSink :public Sink {

public:
    AACMediaSink(RtspContext* ctx, std::unique_ptr<MediaSource> media_source) :Sink(ctx, std::move(media_source), RTP_PAYLOAD_TYPE_AAC) {
        LOG_INFO("media_sink init, %p", media_source_.get());
    }
    ~AACMediaSink() {

    }

    virtual std::string getMediaDescription(uint16_t port) {

        std::string ret = "";
        ret = ret + "m=audio " + std::to_string(port) + " RTP/AVP 97\r\n";
        return ret;

    }
    virtual std::string getAttribute() {

        std::string ret = "";
        ret = ret + "a=rtpmap:97 mpeg4-generic/48000/2\r\n";
        ret += "a=fmtp:97 streamtype=5;profile-level-id=1;sizeLength=13;IndexLength=3;indexDeltaLength=3;mode=AAC-hbr;config=2190;\r\n";
        

        return ret;
    }

protected:
    virtual void sendFrame(std::vector<unsigned char> frame_vec) {

     
        if (frame_vec.size() == 0) {
            return;
        }
        int sendBytes = 0;

        //LOG_INFO("frame_Size=%d \n", (int)frame_vec.size());

       
        RtpPacket* rtpPacket = (RtpPacket*)&buf[0];
        size_t frameSize = frame_vec.size();

        rtpPacket->rtpHeader = rtp_header_;
        rtpPacket->payload[0] = 0x00;
        rtpPacket->payload[1] = 0x10;
        rtpPacket->payload[2] = (frameSize & 0x1FE0) >> 5; //高8位
        rtpPacket->payload[3] = (frameSize & 0x1F) << 3; //低5位
        rtpPacket->size = 4+frameSize;


        memcpy(rtpPacket->payload+4, frame_vec.data(), frameSize);


        sendRtpPacket(rtpPacket);

        rtp_header_.seq++;
        sendBytes += frameSize;
        
        rtp_header_.timestamp += 1024;



        //LOG_INFO("sent bytes: %d, cseq: %d", sendBytes, (int)rtp_header_.seq);
        //latch_.unlock();

    }


};




