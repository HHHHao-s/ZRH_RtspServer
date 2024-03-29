﻿#include "live/RtspServer.h"
#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include "helper/LOG.h"
#include "helper/SocketHelper.h"
#include <unistd.h>

#include <thread>
#include <assert.h>
	

RtspServer::RtspServer(RtspContext* ctx, std::unique_ptr<MediaSessionManager>media_session_manager) : ctx_(ctx), media_session_manager_(std::move(media_session_manager)){
	this->socket_fd_ = OpenListenfd(this->port_);
	if (this->socket_fd_ < 0) {
		LOG_ERROR("OpenListenfd error\n");
	}
	io_event_ = std::make_shared<IOEvent>(socket_fd_, this);
	io_event_->enableReadHandling();
	io_event_->setReadCallback(readCb);

	if (!ctx_->scheduler_->addIOEvent(io_event_)) {
		LOG_ERROR("addIOEvent error\n");
	}

	close_trigger_event_ = std::make_shared<TriggerEvent>( this);
	close_trigger_event_->setTriggerCallback(cbCloseConnect);



}

void RtspServer::Start() {

	LOG_INFO("RtspServer start rtsp://127.0.0.1:%s\n", port_);

	ctx_->scheduler_->loop();

}

//void RtspServer::udpReadCb(void* rtsp_server) {
//	LOG_INFO("RtspServer udpReadCb");
//	RtspServer* server = (RtspServer*)rtsp_server;
//	server->handleUdpRead();
//}
//
//void RtspServer::handleUdpRead() {
//	sockaddr_storage client_addr;
//	memset(&client_addr, 0, sizeof(sockaddr_storage));
//	socklen_t client_len = sizeof(sockaddr_storage);
//
//	int connfd = Accept(this->socket_fd_, (sockaddr*)&client_addr ,&client_len);
//	if (connfd < 0) {
//		LOG_ERROR("Accept error\n");
//	}
//	LOG_INFO("Accept a new DataPacket\n");
//
//
//	
//}

void RtspServer::readCb(void * rtsp_server) {
	LOG_INFO("RtspServer readCb");
	RtspServer * server = (RtspServer *)rtsp_server;
	server->handleRead();
}

void RtspServer::handleRead() {

	int connfd = Accept(this->socket_fd_, NULL, NULL);
	if (connfd < 0) {
		LOG_ERROR("Accept error\n");
	}
	LOG_INFO("Accept a new connection\n");

	
	std::shared_ptr<RtspConnection> conn = std::make_shared<RtspConnection>(ctx_, connfd, this);
	conn->setDisconnectCallback(cbDisConnect);
	conn->setSessionAddCallback(cbSessionAddRtpConn, this);
	conn->setSessionRemoveCallback(cbSessionRemoveRtpConn, this);
	conn->setMediaSessionManagerPtr(media_session_manager_.get());
	conn->setRtspCloseCb(std::bind(&RtspServer::handleDisConnect, this, std::placeholders::_1));
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
		LOG_INFO("%d closed", clientFd);
	}

	disconnect_list_.clear();

}

RtspServer::~RtspServer()
{
	ctx_->scheduler_->removeIOEvent(io_event_);
	Close(this->socket_fd_);

}


void RtspServer::cbSessionAddRtpConn(void* th,TrackId track_id , RtpConnection* rtp_connection,const std::string& session_name) {
	RtspServer* server = (RtspServer*)th;
	server->handleSessionAddRtpConn(track_id,  rtp_connection, session_name);
}

void RtspServer::handleSessionAddRtpConn(TrackId track_id, RtpConnection* rtp_connection, const std::string& session_name) {
	auto media_session = media_session_manager_->LookMediaSession(session_name);
	if (media_session.get() == nullptr) {
		LOG_ERROR("media_session_==nullptr");
	}		
	else {
		media_session->AddRtpConnection(track_id, (RtpConnection*)rtp_connection);
		LOG_INFO("add rtp connection");
	}
	
}

void RtspServer::cbSessionRemoveRtpConn(void* th, TrackId track_id, RtpConnection* rtp_connection, const std::string& session_name) {

	RtspServer* server = (RtspServer*)th;
	server->handleSessionRemoveRtpConn(track_id, rtp_connection, session_name);

}
void RtspServer::handleSessionRemoveRtpConn(TrackId track_id, RtpConnection* rtp_connection, const std::string& session_name) {
	auto media_session = media_session_manager_->LookMediaSession(session_name);
	if (media_session.get() == nullptr) {
		LOG_ERROR("media_session_==nullptr");
	}	
	else {
		media_session->RemoveRtpConnection(track_id, rtp_connection);
		LOG_INFO("remove rtp connection");
	}
	

}