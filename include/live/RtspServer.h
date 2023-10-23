#pragma once
#include "helper/RtspContext.h"
#include "helper/LOG.h"
#include "helper/SocketHelper.h"
#include "helper/Event.h"
#include "live/RtspConnection.h"
#include <unordered_map>
#include "live/MediaSession.h"	
#include "live/MediaSessionManager.h"

class RtspConnection;
class RtspServer
{

public:


	RtspServer(RtspContext* ctx, std::unique_ptr<MediaSessionManager>media_session_manager);
	~RtspServer();
	static void readCb(void *rtsp_server);

	void handleRead();
	static void cbDisConnect(void* arg, int clientFd);
	void handleDisConnect(int clientFd);
	static void cbCloseConnect(void* arg);
	void handleCloseConnect();
	//static void udpReadCb(void* rtsp_server);
	//void handleUdpRead();

	void Start();
	
	

private:
	int socket_fd_;
	std::shared_ptr<IOEvent> io_event_;
	RtspContext* ctx_;
	
	std::unique_ptr<MediaSessionManager> media_session_manager_;

	static void cbSessionAddRtpConn(void *th,TrackId track_id, RtpConnection* rtp_connection, const std::string& session_name);
	 
	void handleSessionAddRtpConn(TrackId track_id, RtpConnection* rtp_connection, const std::string& session_name);
	
	static void cbSessionRemoveRtpConn(void *th, TrackId track_id, RtpConnection* rtp_connection, const std::string& session_name);
	void handleSessionRemoveRtpConn(TrackId track_id, RtpConnection* rtp_connection, const std::string& session_name);

	std::unordered_map<int, std::shared_ptr<RtspConnection>> fd2conn_;
	
	std::mutex latch_;
	std::vector<int> disconnect_list_;//所有被取消的连接 clientFd
	
	std::shared_ptr<TriggerEvent> close_trigger_event_;// 关闭连接的触发事件

	const char port_[6] = "11451";
	
	
};