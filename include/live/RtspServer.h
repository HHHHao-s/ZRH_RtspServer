#pragma once
#include "helper/RtspContext.h"
#include "helper/LOG.h"
#include "helper/SocketHelper.h"
class RtspServer
{

public:


	RtspServer(std::shared_ptr<RtspContext> ctx) : ctx_(ctx) {
		this->socket_fd_ = OpenListenfd(this->port_);
		if (this->socket_fd_ < 0) {
			LOG_ERROR("OpenListenfd error\n");
		}
	}
	~RtspServer();
	void Start();
	

private:
	std::shared_ptr<RtspContext> ctx_;

	char port_[6] = "11451";


	int socket_fd_;
};