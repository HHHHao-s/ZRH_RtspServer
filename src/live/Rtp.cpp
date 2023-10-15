#include "live/Rtp.h"
#include "helper/SocketHelper.h"
#include "helper/LOG.h"


RtpConnection::RtpConnection(RtspContext * ctx, int tcp_fd, uint8_t rtpChannel):ctx_(ctx),tcp_fd_(tcp_fd)
{
	this->alive_ = true;
    rtp_channel_ = rtpChannel;
}

RtpConnection::~RtpConnection()
{
   
}

//void RtpConnection::TimeOutCb(void* t) {
//    RtpConnection* rtpp = static_cast<RtpConnection*>(t);
//    
//    
//    
//}

int RtpConnection::SendFrame(RtpPacket *packet) {
    if(!alive_)
		return -1;
    //latch_.lock();
    
    return SendPackeyOverTcp(packet);

    
}

int RtpConnection::SendPackeyOverTcp(RtpPacket* rtpPacket) {
    

    uint32_t rtpSize = RTP_HEADER_SIZE + rtpPacket->size;
    rtpPacket->prefix[0] = 0x24;//$
    rtpPacket->prefix[1] = 0x00;// track id
    rtpPacket->prefix[2] = (uint8_t)(((rtpSize) & 0xFF00) >> 8);
    rtpPacket->prefix[3] = (uint8_t)((rtpSize) & 0xFF);

    int ret = send(tcp_fd_, rtpPacket->prefix, 4 + rtpSize, 0);


    //LOG_INFO("sent size:%d", ret);
    return ret;
}