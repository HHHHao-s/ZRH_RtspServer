#include "live/Rtp.h"
#include "helper/SocketHelper.h"
#include "helper/LOG.h"


RtpConnection::RtpConnection(RtspContext * ctx, int tcp_fd, uint16_t rtpChannel):ctx_(ctx), sock_fd_(tcp_fd)
{
	this->alive_ = false;
    rtp_channel_ = rtpChannel;
    is_tcp_ = true;
}


RtpConnection::RtpConnection(RtspContext* ctx, int  tcp_fd, uint16_t remote_port, bool is_udp) {
    this->alive_ = false;
	ctx_ = ctx;
    remote_rtp_port_ = remote_port;
    remote_rtcp_port_ = remote_port + 1;

    socklen_t len = sizeof(sockaddr_storage);

    // get remote address
    GetPeerName(tcp_fd, (sockaddr*)&remote_addr_, &len);

    sockaddr_in* sock_addr = (sockaddr_in*)&remote_addr_;

    in_addr_t remote_addr = sock_addr->sin_addr.s_addr;

    memset(sock_addr, 0, sizeof(sockaddr_in));
    sock_addr->sin_port = htons(remote_port);
    sock_addr->sin_addr.s_addr = remote_addr;
    sock_addr->sin_family = AF_INET;



    ShowAddress(sock_addr);

    sock_fd_ = OpenClientUdp(&local_rtp_port_);
    rtcp_sock_fd_ = OpenAcquireClientUdp(local_rtcp_port_);
    


	is_tcp_ = false;

}


RtpConnection::~RtpConnection()
{
   
}



int RtpConnection::SendFrame(RtpPacket *packet) {
    if(!alive_)
		return -1;
    //latch_.lock();
    if (is_tcp_) {
        return SendPacketOverTcp(packet);
    }
    else {
        return SendPacketOverUdp(packet);
    }
    

    
}

int RtpConnection::SendPacketOverTcp(RtpPacket* rtpPacket) {
    

    uint32_t rtpSize = RTP_HEADER_SIZE + rtpPacket->size;
    rtpPacket->prefix[0] = 0x24;//$
    rtpPacket->prefix[1] = rtp_channel_;// channel id
    rtpPacket->prefix[2] = (uint8_t)(((rtpSize) & 0xFF00) >> 8);
    rtpPacket->prefix[3] = (uint8_t)((rtpSize) & 0xFF);

    int ret = send(sock_fd_, rtpPacket->prefix, 4 + rtpSize, 0);


    //LOG_INFO("sent size:%d", ret);
    return ret;
}

int RtpConnection::SendPacketOverUdp(RtpPacket* rtpPacket) {


    uint32_t rtpSize = RTP_HEADER_SIZE + rtpPacket->size;
    
    int ret = SendTo(sock_fd_, &rtpPacket->rtpHeader, rtpSize, 0,(sockaddr*)&remote_addr_, sizeof(sockaddr_in));


    //LOG_INFO("sent size:%d", ret);
    return ret;
}