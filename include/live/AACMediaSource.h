#pragma once
#include <unistd.h>
#include "helper/SocketHelper.h"
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <string_view>
#include <mutex>
#include <list>
#include <stdint.h>
#include "helper/RtspContext.h"
#include "helper/RingBuffer.h"
#include "live/MediaSource.h"
#define MAX_FRAME_SIZE 1024 * 1024
#define AAC_FRAME_SIZE_FPS 46

class AACMediaSource;

struct AdtsHeader
{
    uint16_t syncword;  //12 bit 同步字 '1111 1111 1111'，说明一个ADTS帧的开始
    uint8_t id;        //1 bit MPEG 标示符， 0 for MPEG-4，1 for MPEG-2
    uint8_t layer;     //2 bit 总是'00'
    uint8_t protectionAbsent;  //1 bit 1表示没有crc，0表示有crc
    uint8_t profile;           //1 bit 表示使用哪个级别的AAC
    uint8_t samplingFreqIndex; //4 bit 表示使用的采样频率
    uint8_t privateBit;        //1 bit
    uint8_t channelCfg; //3 bit 表示声道数
    uint8_t originalCopy;         //1 bit 
    uint8_t home;                  //1 bit 

    /*下面的为改变的参数即每一帧都不同*/
    uint8_t copyrightIdentificationBit;   //1 bit
    uint8_t copyrightIdentificationStart; //1 bit
    uint16_t aacFrameLength;               //13 bit 一个ADTS帧的长度包括ADTS头和AAC原始流
    uint16_t adtsBufferFullness;           //11 bit 0x7FF 说明是码率可变的码流

    /* number_of_raw_data_blocks_in_frame
     * 表示ADTS帧中有number_of_raw_data_blocks_in_frame + 1个AAC原始帧
     * 所以说number_of_raw_data_blocks_in_frame == 0
     * 表示说ADTS帧中有一个AAC数据块并不是说没有。(一个AAC原始帧包含一段时间内1024个采样及相关数据)
     */
    uint8_t numberOfRawDataBlockInFrame; //2 bit
};



//void CacheTask(H264MediaSource* t);

class AACMediaSource :public MediaSource
{
	//friend void CacheTask(H264MediaSource* t);
public:
	AACMediaSource(RtspContext* ctx, std::string_view file_name);
	~AACMediaSource();

	/*	Read one frame from file then store it in frame
	*/
	std::vector<unsigned char> ReadFrame();
    // 没有adst头
	std::vector<unsigned char> GetFrameFromFile();

private:


    int parseAdtsHeader(const unsigned char* in, struct AdtsHeader* res)
    {
        
        memset(res, 0, sizeof(*res));

        if ((in[0] == 0xFF) && ((in[1] & 0xF0) == 0xF0))
        {
            res->id = ((unsigned int)in[1] & 0x08) >> 3;
            res->layer = ((unsigned int)in[1] & 0x06) >> 1;
            res->protectionAbsent = (unsigned int)in[1] & 0x01;
            res->profile = ((unsigned int)in[2] & 0xc0) >> 6;
            res->samplingFreqIndex = ((unsigned int)in[2] & 0x3c) >> 2;
            res->privateBit = ((unsigned int)in[2] & 0x02) >> 1;
            res->channelCfg = ((((unsigned int)in[2] & 0x01) << 2) | (((unsigned int)in[3] & 0xc0) >> 6));
            res->originalCopy = ((unsigned int)in[3] & 0x20) >> 5;
            res->home = ((unsigned int)in[3] & 0x10) >> 4;
            res->copyrightIdentificationBit = ((unsigned int)in[3] & 0x08) >> 3;
            res->copyrightIdentificationStart = (unsigned int)in[3] & 0x04 >> 2;
            res->aacFrameLength = (((((unsigned int)in[3]) & 0x03) << 11) |
                (((unsigned int)in[4] & 0xFF) << 3) |
                ((unsigned int)in[5] & 0xE0) >> 5);
            res->adtsBufferFullness = (((unsigned int)in[5] & 0x1f) << 6 |
                ((unsigned int)in[6] & 0xfc) >> 2);
            res->numberOfRawDataBlockInFrame = ((unsigned int)in[6] & 0x03);

            return 0;
        }
        else
        {
            return -1;
        }
    }

	void CacheFrame();


	RingBuffer ring_buffer_;
	enum
	{
		START_CODE3,
		START_CODE4
	}start_code_;

	std::queue<std::vector<unsigned char>> frames_;
	size_t max_frame_size_{ 8 };


};