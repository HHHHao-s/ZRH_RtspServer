#pragma once
#include "helper/RtspContext.h"
#include "helper/LOG.h"
#include "helper/SocketHelper.h"
#include "helper/Event.h"
#include "live/RtspConnection.h"
#include <unordered_map>
#include "live/MediaSession.h"	

class RtspConnection;
class RtspServer
{

public:


	RtspServer(RtspContext* ctx, MediaSession *media_session);
	~RtspServer();
	static void readCb(void *rtsp_server);

	void handleRead();
	static void cbDisConnect(void* arg, int clientFd);
	void handleDisConnect(int clientFd);
	static void cbCloseConnect(void* arg);
	void handleCloseConnect();
	
	void Start();
	
	

private:
	int socket_fd_;
	std::shared_ptr<IOEvent> io_event_;
	RtspContext* ctx_;
	MediaSession* media_session_;
	static void cbSessionAddRtpConn(void *th,TrackId track_id, RtpConnection* rtp_connection);
	 
	void handleSessionAddRtpConn(TrackId track_id, RtpConnection* rtp_connection);
	
	static void cbSessionRemoveRtpConn(void *th, TrackId track_id, RtpConnection* rtp_connection);
	void handleSessionRemoveRtpConn(TrackId track_id, RtpConnection* rtp_connection);

	std::unordered_map<int, std::shared_ptr<RtspConnection>> fd2conn_;
	
	std::mutex latch_;
	std::vector<int> disconnect_list_;//所有被取消的连接 clientFd
	
	std::shared_ptr<TriggerEvent> close_trigger_event_;// 关闭连接的触发事件

	char port_[6] = "11451";

	
	
};