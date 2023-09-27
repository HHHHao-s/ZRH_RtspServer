#pragma once
#include <stdint.h>
#include <string>
#include "H264MediaSource.h"
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
    struct RtpHeader rtpHeader;
    uint8_t payload[0];
};

class RtpConnection
{
public:
    
    RtpConnection(std::shared_ptr<RtspContext> ctx, int tcp_fd , std::string file_name);
    ~RtpConnection();

    int SendFrame();

    

private:
    std::shared_ptr<RtspContext> ctx_;
    int SendPackeyOverTcp(RtpPacket* rtpPacket, int size);

    int tcp_fd_;
    H264MediaSource h264_media_source_;
    RtpHeader rtp_header_;
	
	bool alive_;
    

};