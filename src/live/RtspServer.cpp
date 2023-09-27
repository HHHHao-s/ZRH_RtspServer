#include "live/RtspServer.h"
#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include "helper/LOG.h"
#include "helper/SocketHelper.h"
#include <unistd.h>
#include "live/RtspConnection.h"
#include <thread>
	

void RtspServer::Start() {

	LOG_INFO("RtspServer start rtsp://127.0.0.1:%s\n", port_);

	while (1) {
		int connfd = Accept(this->socket_fd_, NULL, NULL);
		if (connfd < 0) {
			LOG_ERROR("Accept error\n");
			continue;
		}
		LOG_INFO("Accept a new connection\n");

		std::thread t([=]() {
			RtspConnection* conn = new RtspConnection(ctx_, connfd);
			conn->run();
			});
		t.detach();

		

	}
}


RtspServer::~RtspServer()
{
	//dtor
}

