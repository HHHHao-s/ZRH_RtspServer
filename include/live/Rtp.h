#pragma once
#include <stdint.h>
#include <string>
#include "H264MediaSource.h"
#include "helper/Event.h"
#include <mutex>
#define RTP_MAX_PKT_SIZE 1400
#define RTP_HEADER_SIZE 12

struct RtpHeader
{
    /* byte 0 */
    uint8_t csrcLen : 4;
    uint8_t extension : 1;
    uint8_t padding : 1;
    uint8_t version : 2;

    /* byte 1 */
    uint8_t payloadType : 7;
    uint8_t marker : 1;

    /* bytes 2,3 */
    uint16_t seq;

    /* bytes 4-7 */
    uint32_t timestamp;

    /* bytes 8-11 */
    uint32_t ssrc;
};



struct RtpPacket
{
    size_t size; // payload size
    uint8_t prefix[4];
    struct RtpHeader rtpHeader;
    uint8_t payload[0];
};

class RtpConnection
{
public:
    
    RtpConnection(RtspContext * ctx, int tcp_fd, uint16_t rtpChannel);
    RtpConnection(RtspContext* ctx, int tcp_fd, uint16_t remote_port, bool udp);
    ~RtpConnection();

    
    void setAlive(bool alive) {
		alive_ = alive;
	}
    
    int SendFrame(RtpPacket *packet);

    uint16_t getLocalPort() {
		return local_rtp_port_;
	}
    

private:
    int SendPacketOverTcp(RtpPacket* rtpPacket);
    int SendPacketOverUdp(RtpPacket* rtpPacket);
    RtspContext * ctx_;
    int sock_fd_;
    int rtcp_sock_fd_;
	bool alive_;

    uint16_t rtp_channel_;
    std::mutex latch_;
    bool is_tcp_;
    
    uint16_t remote_rtp_port_;
    uint16_t remote_rtcp_port_;
    uint16_t local_rtp_port_;
    uint16_t local_rtcp_port_;
    sockaddr_storage remote_addr_;
};