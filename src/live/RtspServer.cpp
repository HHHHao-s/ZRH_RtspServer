#include "live/RtspServer.h"
#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include "helper/LOG.h"
#include "helper/SocketHelper.h"
#include <unistd.h>
#include "live/RtspConnection.h"
#include <thread>
#include <assert.h>
	

RtspServer::RtspServer(std::shared_ptr<RtspContext> ctx) : ctx_(ctx) {
	this->socket_fd_ = OpenListenfd(this->port_);
	if (this->socket_fd_ < 0) {
		LOG_ERROR("OpenListenfd error\n");
	}
	std::shared_ptr<IOEvent> io_event = std::make_shared<IOEvent>(socket_fd_, this);
	io_event->enableReadHandling();
	io_event->setReadCallback(readCb);

	if (!ctx_->scheduler_->addIOEvent(io_event)) {
		LOG_ERROR("addIOEvent error\n");
	}

	close_trigger_event_ = std::make_shared<TriggerEvent>( this);
	close_trigger_event_->setTriggerCallback(cbCloseConnect);


}

void RtspServer::Start() {

	LOG_INFO("RtspServer start rtsp://127.0.0.1:%s\n", port_);

	ctx_->scheduler_->loop();

}

void RtspServer::readCb(void * rtsp_server) {
	LOG_INFO("RtspServer readCb");
	RtspServer * server = (RtspServer *)rtsp_server;
	server->handleRead();
}

void RtspServer::handleRead() {
	LOG_INFO("RtspServer handleRead\n");
	int connfd = Accept(this->socket_fd_, NULL, NULL);
	if (connfd < 0) {
		LOG_ERROR("Accept error\n");
	}
	LOG_INFO("Accept a new connection\n");

	
	std::shared_ptr<RtspConnection> conn = std::make_shared<RtspConnection>(ctx_, connfd, this);
	conn->setDisconnectCallback(cbDisConnect);
	fd2conn_[connfd] = conn;
		

}

void RtspServer::cbDisConnect(void* arg, int clientFd) {
	RtspServer* server = (RtspServer*)arg;

	server->handleDisConnect(clientFd);
}

void RtspServer::handleDisConnect(int clientFd) {

	std::lock_guard <std::mutex> lck(latch_);
	disconnect_list_.push_back(clientFd);

	if (disconnect_list_.size() == 1) {// 只需要添加一次触发事件
		ctx_->scheduler_->addTriggerEvent(close_trigger_event_);
	}
}

void RtspServer::cbCloseConnect(void* arg) {
	RtspServer* server = (RtspServer*)arg;
	server->handleCloseConnect();
}
void RtspServer::handleCloseConnect() {

	std::lock_guard <std::mutex> lck(latch_);

	for (std::vector<int>::iterator it = disconnect_list_.begin(); it != disconnect_list_.end(); ++it) {

		int clientFd = *it;

		assert(fd2conn_.count(clientFd)==1);
		fd2conn_.erase(clientFd);
	}

	disconnect_list_.clear();

}

RtspServer::~RtspServer()
{
	//dtor
}

