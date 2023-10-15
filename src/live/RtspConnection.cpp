#include "live/RtspConnection.h"
#include "helper/SocketHelper.h"
#include <stdio.h>
#include <unistd.h>
#include "helper/LOG.h"
#include <time.h>
#include <string.h>
#include <sstream>
#include <iostream>

#include "live/H264MediaSource.h"
#include "helper/Event.h"




bool RtspConnection::handleSetup() {
	


	size_t pos=std::string::npos;
	if ((pos = rbuf_.find("RTP/AVP/TCP")) != std::string::npos) {
		LOG_INFO("RTP/AVP/TCP");
	}
	else if ((pos = rbuf_.find("RTP/AVP")) != std::string::npos) {
		LOG_INFO("RTP/AVP");
	}
	else {
		LOG_INFO("RTP/AVP/UDP");
	
	}

	


	char result[1024];
	sprintf(result, "RTSP/1.0 200 OK\r\n"
		"CSeq: %d\r\n"
		"Transport: RTP/AVP/TCP;unicast;interleaved=0-1\r\n"
		"Session: 66334873\r\n"
		"\r\n",
		cseq_);
	write(client_fd_, result, strlen(result));
	LOG_INFO("%s", result);
	return 0;
}

bool RtspConnection::handleTeardown() {
	char result[1024];
	sprintf(result, "RTSP/1.0 200 OK\r\n"
		"CSeq: %d\r\n"
		"\r\n",
		cseq_);
	write(client_fd_, result, strlen(result));
	LOG_INFO("%s", result);
	alive_ = false;
	return 0;
}

bool RtspConnection::handlePlay() {
	char result[1024];
	sprintf(result, "RTSP/1.0 200 OK\r\n"
		"CSeq: %d\r\n"
		"Range: npt=0.000-\r\n"
		"Session: 66334873; timeout=10\r\n\r\n",
		cseq_);
	write(client_fd_, result, strlen(result));
	LOG_INFO("%s", result);

	//LOG_INFO("%s", ROOT_DIR  "/data/test.h264");


	
	auto p = std::make_shared<RtpConnection>(ctx_, this->client_fd_, 0);

	rtp_conns_.push_back(p);

	session_add_cb(session_ptr_, TrackId0, p.get());
	

	
	return 0;
}

bool RtspConnection::handleDescribe() {
	char result[1024];
	char sdp[500];
	char localIp[100];

	sscanf(url_, "rtsp://%[^:]:", localIp);

	sprintf(sdp, "v=0\r\n"
		"o=- 9%ld 1 IN IP4 %s\r\n"
		"t=0 0\r\n"
		"a=control:*\r\n"
		"m=video 0 RTP/AVP/TCP 96\r\n"
		"a=rtpmap:96 H264/90000\r\n"
		"a=control:track0\r\n",
		time(NULL), localIp);

	sprintf(result, "RTSP/1.0 200 OK\r\nCSeq: %d\r\n"
		"Content-Base: %s\r\n"
		"Content-type: application/sdp\r\n"
		"Content-length: %zu\r\n\r\n"
		"%s",
		cseq_,
		url_,
		strlen(sdp),
		sdp);

	write(client_fd_, result, strlen(result));
	LOG_INFO("%s", result);
	return 0;
}

bool RtspConnection::handleOptions() {
	char result[1024];
	sprintf(result, "RTSP/1.0 200 OK\r\n"
		"CSeq: %d\r\n"
		"Public: OPTIONS, DESCRIBE, SETUP, PLAY\r\n"
		"\r\n",
		cseq_);
	write(client_fd_, result, strlen(result));
	LOG_INFO("%s", result);
	return 0;
}


RtspConnection::RtspConnection(RtspContext * ctx, int client_fd, RtspServer* server) :ctx_(ctx), client_fd_(client_fd), server_(server), alive_(true)
{
	LOG_INFO("RtspConnection::RtspConnection()");
	io_event_ = std::make_shared<IOEvent>(client_fd_, this);
	io_event_->enableReadHandling();
	io_event_->setReadCallback(readCallback);

	if (!ctx_->scheduler_->addIOEvent(io_event_)) {
		LOG_ERROR("addIOEvent error\n");
	}
	rbuf_.resize(buf_size_);
}

RtspConnection::~RtspConnection()
{
	if (!ctx_->scheduler_->removeIOEvent(io_event_)) {
		LOG_ERROR("removeIOEvent error\n");
	}
	LOG_INFO("RtspConnection::~RtspConnection()");
	

	for (auto& rtp_conn : rtp_conns_)
		session_remove_cb(session_ptr_, TrackId0, rtp_conn.get());

	if (client_fd_ > 0)
		Close(client_fd_);

}


//int RtspConnection::playLoop() {
//#ifndef ROOT_DIR
//#define ROOT_DIR "../"	
//#endif // ROOT_DIR
//
//	LOG_INFO("%s", ROOT_DIR  "/data/test.h264");
//
//	
//
//	RtpConnection rtp_connection(ctx_, this->client_fd_, ROOT_DIR  "/data/test.h264");
//	
//	while (1) {	
//		//LOG_INFO("begin send");	
//		if (rtp_connection.SendFrame() == -1) {
//			
//			break;
//		}
//		//LOG_INFO("begin sleep");
//		usleep(40);// 25fps
//		//LOG_INFO("end sleep");
//
//	}
//	return 0;
//}

void RtspConnection::readCallback(void *conn) {
	RtspConnection* rtsp_connection = static_cast<RtspConnection*>(conn);
	rtsp_connection->handleRtspRequest();
}

bool RtspConnection::parseRequestMethod(const char* line) {
	if (strncmp(line, "DESCRIBE", sizeof("DESCRIBE") - 1) == 0) {
		method_ = Method::DESCRIBE;

	}
	else if (strncmp(line, "SETUP", sizeof("SETUP") - 1) == 0) {
		method_ = Method::SETUP;

	}
	else if (strncmp(line, "PLAY", sizeof("PLAY") - 1) == 0) {
		method_ = Method::PLAY;

	}
	else if (strncmp(line, "TEARDOWN", sizeof("TEARDOWN") - 1) == 0) {
		method_ = Method::TEARDOWN;

	}
	else if (strncmp(line, "OPTIONS", sizeof("OPTIONS") - 1) == 0) {
		method_ = Method::OPTIONS;

	}
	else {
		method_ = Method::UNKNOWN;
		return false;
	}

	if (sscanf(line, "%s %s %s", method_buf_, url_, version_) != 3) {
		return false;
	}
		

	if (strncmp(url_, "rtsp://", 7) != 0)
	{
		return false;
	}

	if (sscanf(url_ + 7, "%[^:]:%hu/%s", ip_, &port_, suffix_) == 3)
    {

    }
    else if (sscanf(url_ + 7, "%[^/]/%s", ip_, suffix_) == 2)
    {
        port_ = 554;// 如果rtsp请求地址中无端口，默认获取的端口为：554
    }
    

	return true;
}

bool RtspConnection::parseRequestLine(const char* line) {
	// to do
	return false;
}

int RtspConnection::handleRtspRequest() {

	
	
	int n = recv(client_fd_, static_cast<void*>(const_cast<char*>(rbuf_.c_str())), buf_size_, MSG_DONTWAIT);
	

	if (n == -1) {
		LOG_ERROR("read error");
		return -1;
	}
	else if (n == 0) {
		LOG_INFO("client close");
		alive_ = false;
		if (disconnect_cb_)
			disconnect_cb_(server_, client_fd_);
		return -1;
	}
	else {
		while (n == buf_size_) {
			std::string buf(buf_size_, '\0');
			int tmp = read(client_fd_, static_cast<void*>(const_cast<char*>(buf.c_str())), buf_size_);
			if (tmp == -1) {
				LOG_ERROR("read error");
				return -1;
			}
			else if (tmp == 0) {
				LOG_INFO("client close");
				alive_ = false;
				if (disconnect_cb_)
					disconnect_cb_(server_, client_fd_);
				return -1;
			}
			else {
				rbuf_ += buf;
				n += tmp;
			}
			buf_size_ *= 2;
		}

		LOG_INFO("read %d bytes", n);
		LOG_INFO("read data: %s", rbuf_.c_str());
		rbuf_[n] = '\0';

		std::stringstream iss(rbuf_);
		std::string line;

		
		std::getline(iss, line);
		if (line.size() == 0) {
			return 0;
		}
		LOG_INFO("line: %s", line.c_str());
		const char* p = line.c_str();
		if (!parseRequestMethod(p)) {
			LOG_INFO("method wrong");
			return false;
		}
		

		switch (method_)
		{
		case RtspConnection::Method::OPTIONS:
			return handleOptions();
		case RtspConnection::Method::DESCRIBE:
			return handleDescribe();
		case RtspConnection::Method::SETUP:
			return handleSetup();
		case RtspConnection::Method::PLAY:
			return handlePlay();
		case RtspConnection::Method::TEARDOWN:
			return handleTeardown();
		case RtspConnection::Method::UNKNOWN:
			break;
		default:
			break;
		}

	}


	return 0;


}

