#include "RstpServer.h"
#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include "LOG.h"
#include "SocketHelper.h"
	

RstpServer::RstpServer()
{
	this->socket_fd_ = OpenListenfd(this->port_);
	if (this->socket_fd_ < 0) {
		LOG_ERROR("OpenListenfd error\n");
	}
}

RstpServer::~RstpServer()
{
	//dtor
}

