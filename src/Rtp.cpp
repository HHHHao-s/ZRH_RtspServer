#include "Rtp.h"
#include "SocketHelper.h"
#include "LOG.h"


RtpConnection::RtpConnection(int tcp_fd, std::string file_name):tcp_fd_(tcp_fd), h264_media_source_(file_name)
{
	this->alive_ = true;
    frame_.data = (char*)malloc(1024 * 1024);

	rtp_header_.csrcLen = 0;
	rtp_header_.extension = 0;
	rtp_header_.padding = 0;
	rtp_header_.version = 2;
	rtp_header_.payloadType = 96;
	rtp_header_.marker = 0;
	rtp_header_.seq = 0;
	rtp_header_.timestamp = 0;
	rtp_header_.ssrc = 0;

}

RtpConnection::~RtpConnection()
{
	free(frame_.data);
	frame_.data = NULL;
}

int RtpConnection::SendFrame() {
	
	int ret = h264_media_source_.ReadFrame(&frame_);
    if (ret == -1) {
        return -1;
    }


    uint8_t naluType; // nalu第一个字节
    int sendBytes = 0;


    naluType = frame_.data[0];

    LOG_INFO("frame_Size=%d \n", (int)frame_.size);

    RtpPacket *rtpPacket = (RtpPacket *)malloc(sizeof(RtpPacket) + frame_.size);
    rtpPacket->rtpHeader = rtp_header_;

    if (frame_.size <= RTP_MAX_PKT_SIZE) // nalu长度小于最大包长：单一NALU单元模式
    {

        //*   0 1 2 3 4 5 6 7 8 9
        //*  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //*  |F|NRI|  Type   | a single NAL unit ... |
        //*  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

        memcpy(rtpPacket->payload, frame_.data, frame_.size);
        ret = SendPackeyOverTcp(rtpPacket, sizeof(RtpPacket) + frame_.size);
        if (ret < 0){
            LOG_ERROR("send error%d", ret);
            return -1;
        }
            

        rtp_header_.seq++;
        sendBytes += ret;
        if ((naluType & 0x1F) == 7 || (naluType & 0x1F) == 8) // 如果是SPS、PPS就不需要加时间戳
            goto out;
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


        int pktNum = frame_.size / RTP_MAX_PKT_SIZE;       // 有几个完整的包
        int remainPktSize = frame_.size % RTP_MAX_PKT_SIZE; // 剩余不完整包的大小
        int i, pos = 1;

        // 发送完整的包
        for (i = 0; i < pktNum; i++)
        {
            rtpPacket->payload[0] = (naluType & 0x60) | 28;
            rtpPacket->payload[1] = naluType & 0x1F;

            if (i == 0) //第一包数据
                rtpPacket->payload[1] |= 0x80; // start
            else if (remainPktSize == 0 && i == pktNum - 1) //最后一包数据
                rtpPacket->payload[1] |= 0x40; // end

            memcpy(rtpPacket->payload + 2, frame_.data + pos, RTP_MAX_PKT_SIZE);
            ret = SendPackeyOverTcp(rtpPacket, RTP_MAX_PKT_SIZE + 2);
            if (ret < 0)
                return -1;

            rtpPacket->rtpHeader.seq++;
            sendBytes += ret;
            pos += RTP_MAX_PKT_SIZE;
        }

        // 发送剩余的数据
        if (remainPktSize > 0)
        {
            rtpPacket->payload[0] = (naluType & 0x60) | 28;
            rtpPacket->payload[1] = naluType & 0x1F;
            rtpPacket->payload[1] |= 0x40; //end

            memcpy(rtpPacket->payload + 2, frame_.data + pos, remainPktSize);// 不需要复制加2字节的头
            ret = SendPackeyOverTcp(rtpPacket, remainPktSize + 2);

            if (ret < 0)
                return -1;

            rtpPacket->rtpHeader.seq++;
            sendBytes += ret;
        }
    }
    rtp_header_.timestamp += 90000 / 25;
out:

    LOG_INFO("sent bytes: %d", sendBytes);

    return sendBytes;


}

int RtpConnection::SendPackeyOverTcp(RtpPacket* rtpPacket, int size) {
    rtpPacket->rtpHeader.seq = htons(rtpPacket->rtpHeader.seq);
    rtpPacket->rtpHeader.timestamp = htonl(rtpPacket->rtpHeader.timestamp);
    rtpPacket->rtpHeader.ssrc = htonl(rtpPacket->rtpHeader.ssrc);

    uint32_t rtpSize = RTP_HEADER_SIZE + size;
    char* tempBuf = (char*)malloc(4 + rtpSize);
    tempBuf[0] = 0x24;//$
    tempBuf[1] = 0x00;
    tempBuf[2] = (uint8_t)(((rtpSize) & 0xFF00) >> 8);
    tempBuf[3] = (uint8_t)((rtpSize) & 0xFF);
    memcpy(tempBuf + 4, (char*)rtpPacket, rtpSize);

    int ret = send(tcp_fd_, tempBuf, 4 + rtpSize, 0);

    rtpPacket->rtpHeader.seq = ntohs(rtpPacket->rtpHeader.seq);
    rtpPacket->rtpHeader.timestamp = ntohl(rtpPacket->rtpHeader.timestamp);
    rtpPacket->rtpHeader.ssrc = ntohl(rtpPacket->rtpHeader.ssrc);

    free(tempBuf);
    tempBuf = NULL;
    LOG_INFO("sent size:%d", ret);
    return ret;
}