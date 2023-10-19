#pragma once
#include "H264MediaSource.h"
#include "live/Sink.h"
#include <string>

#define RTP_PAYLOAD_TYPE_H264 96
class H264MediaSink:public Sink {

public:
	H264MediaSink(RtspContext* ctx, std::unique_ptr<MediaSource> media_source) :Sink(ctx, std::move(media_source), RTP_PAYLOAD_TYPE_H264) {
        LOG_INFO("media_sink init, %p", media_source_.get());
	}
	~H264MediaSink() {

	}

	virtual std::string getMediaDescription(uint16_t port) {
        // to do
        std::string ret="";
        ret = ret + "m=video " + std::to_string(port) + " RTP/AVP 96\r\n";
        return ret;

    }
	virtual std::string getAttribute() {
        // to do
        std::string ret = "";
        ret = ret+"a=rtpmap:96 H264/16000\r\n";
        ret += "a=framerate:" + std::to_string(media_source_->GetFPS()) + "\r\n";
        
        return ret;
    }

protected:
	virtual void sendFrame(std::vector<unsigned char> frame_vec) {

        int ret = 0;
        if (frame_vec.size() == 0) {
            return;
        }


        uint8_t naluType; // nalu第一个字节
        int sendBytes = 0;
        naluType = frame_vec[0];
        //LOG_INFO("frame_Size=%d \n", (int)frame_vec.size());

        if (frame_vec.size() <= RTP_MAX_PKT_SIZE) // nalu长度小于最大包长：单一NALU单元模式
        {

            RtpPacket* rtpPacket = (RtpPacket*)&buf[0];
            rtpPacket->rtpHeader = rtp_header_;
            //*   0 1 2 3 4 5 6 7 8 9
            //*  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            //*  |F|NRI|  Type   | a single NAL unit ... |
            //*  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            memcpy(rtpPacket->payload, (void*)&frame_vec[0], frame_vec.size());
            rtpPacket->size = frame_vec.size();

            sendRtpPacket(rtpPacket);



            rtp_header_.seq++;
            sendBytes += ret;
            if ((naluType & 0x1F) == 7 || (naluType & 0x1F) == 8) // 如果是SPS、PPS就不需要加时间戳
                return;
        }
        else // nalu长度大于最大包场：分片模式
        {

            //*  0                   1                   2
            //*  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
            //* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            //* | FU indicator  |   FU header   |   FU payload   ...  |
            //* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+



            //*     FU Indicator
            //*    0 1 2 3 4 5 6 7
            //*   +-+-+-+-+-+-+-+-+
            //*   |F|NRI|  Type   |
            //*   +---------------+



            //*      FU Header
            //*    0 1 2 3 4 5 6 7
            //*   +-+-+-+-+-+-+-+-+
            //*   |S|E|R|  Type   |
            //*   +---------------+

            
            int pktNum = frame_vec.size() / RTP_MAX_PKT_SIZE;       // 有几个完整的包
            int remainPktSize = frame_vec.size() % RTP_MAX_PKT_SIZE; // 剩余不完整包的大小
            int i, pos = 1;

            // 发送完整的包
            for (i = 0; i < pktNum; i++)
            {
                RtpPacket* rtpPacket = (RtpPacket*)&buf[0];
                rtpPacket->rtpHeader = rtp_header_;
                rtpPacket->payload[0] = (naluType & 0x60) | 28;
                rtpPacket->payload[1] = naluType & 0x1F;

                if (i == 0) //第一包数据
                    rtpPacket->payload[1] |= 0x80; // start
                else if (remainPktSize == 0 && i == pktNum - 1) //最后一包数据
                    rtpPacket->payload[1] |= 0x40; // end

                memcpy(rtpPacket->payload + 2, (void*)&frame_vec[pos], RTP_MAX_PKT_SIZE);
                rtpPacket->size = RTP_MAX_PKT_SIZE + 2;

                sendRtpPacket(rtpPacket);
                rtp_header_.seq++;
                sendBytes += ret;
                pos += RTP_MAX_PKT_SIZE;
            }

            // 发送剩余的数据
            if (remainPktSize > 0)
            {
                RtpPacket* rtpPacket = (RtpPacket*)&buf[0];
                rtpPacket->rtpHeader = rtp_header_;
                rtpPacket->payload[0] = (naluType & 0x60) | 28;
                rtpPacket->payload[1] = naluType & 0x1F;
                rtpPacket->payload[1] |= 0x40; //end

                memcpy(rtpPacket->payload + 2, (void*)&frame_vec[pos], remainPktSize);// 不需要复制加2字节的头
                rtpPacket->size = remainPktSize + 2;

                sendRtpPacket(rtpPacket);
                rtp_header_.seq++;
                sendBytes += ret;
            }
        }
        rtp_header_.timestamp += 16000 / media_source_->GetFPS();



        //LOG_INFO("sent bytes: %d, cseq: %d", sendBytes, (int)rtp_header_.seq);
        //latch_.unlock();

    }


};




