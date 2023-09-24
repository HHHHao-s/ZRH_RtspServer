#include "RtspConnection.h"
#include "SocketHelper.h"
#include <stdio.h>
#include <unistd.h>
#include "LOG.h"
#include <time.h>
#include <string.h>
#include <sstream>
#include <iostream>



int RtspConnection::handleSetup() {
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

int RtspConnection::handleTeardown() {
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

int RtspConnection::handlePlay() {
	char result[1024];
	sprintf(result, "RTSP/1.0 200 OK\r\n"
		"CSeq: %d\r\n"
		"Range: npt=0.000-\r\n"
		"Session: 66334873; timeout=10\r\n\r\n",
		cseq_);
	write(client_fd_, result, strlen(result));
	LOG_INFO("%s", result);
	return 0;
}

int RtspConnection::handleDescribe() {
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

int RtspConnection::handleOptions() {
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


RtspConnection::RtspConnection(int client_fd) : client_fd_(client_fd), alive_(true)
{
	//ctor
}

RtspConnection::~RtspConnection()
{
	LOG_INFO("RtspConnection::~RtspConnection()");
	if (client_fd_ > 0)
		Close(client_fd_);

}

void RtspConnection::run() {
	while (alive_) {
		if (handleRtspRequest() < 0) {
			alive_ = false;
			break;
		}
	}

}

int RtspConnection::handleRtspRequest() {

	std::string rbuf_str(1024, '\0');


	int n = read(client_fd_, static_cast<void*>(const_cast<char*>(rbuf_str.c_str())), rbuf_str.size());

	if (n == -1) {
		LOG_ERROR("read error");
		return -1;
	}
	else if (n == 0) {
		LOG_INFO("client close");
		alive_ = false;
		return -1;
	}
	else {

		LOG_INFO("read %d bytes", n);
		LOG_INFO("read data: %s", rbuf_str.c_str());
		rbuf_str.resize(n);

		std::stringstream iss(rbuf_str);
		std::string line;

		while (1) {
			std::getline(iss, line);
			if (line.size() == 0) {
				break;
			}
			LOG_INFO("line: %s", line.c_str());
			const char* p = line.c_str();
			if (strncmp(p, "DESCRIBE", sizeof("DESCRIBE") - 1) == 0 ||
				strncmp(p, "SETUP", sizeof("SETUP") - 1) == 0 ||
				strncmp(p, "PLAY", sizeof("PLAY") - 1) == 0 ||
				strncmp(p, "TEARDOWN", sizeof("TEARDOWN") - 1) == 0 ||
				strncmp(p, "OPTIONS", sizeof("OPTIONS") - 1) == 0) {
				if (sscanf(p, "%s %s RTSP/1.0", method_, url_) != 2) {
					LOG_INFO("parse method error");
					return -1;
				}
			}
			else if (strncmp(p, "CSeq", sizeof("CSeq") - 1) == 0) {
				if (sscanf(p, "CSeq: %d", &cseq_) != 1) {
					LOG_INFO("parse CSeq error");
					return -1;
				}

			}
			else if (strncmp(p, "Transport", sizeof("Transport") - 1) == 0) {
				// Transport: RTP/AVP/UDP;unicast;client_port=13358-13359
				// Transport: RTP/AVP;unicast;client_port=13358-13359
				if (sscanf(p, "Transport: RTP/AVP/TCP;unicast;interleaved=0-1\r\n") != 0) {
					LOG_INFO("parse Transport error");
					return -1;
				}
			}
		}


		if (strcmp(method_, "OPTIONS") == 0) {
			return handleOptions();
		}
		else if (strcmp(method_, "DESCRIBE") == 0) {
			return handleDescribe();
		}
		else if (strcmp(method_, "SETUP") == 0) {
			return handleSetup();
		}
		else if (strcmp(method_, "PLAY") == 0) {
			return handlePlay();
		}
		else if (strcmp(method_, "TEARDOWN") == 0) {
			return handleTeardown();
		}
		else {
			return LOG_INFO("method not support");
		}

	}


	return 0;


}

