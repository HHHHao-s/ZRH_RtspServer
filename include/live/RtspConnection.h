#pragma once
#include <memory>
#include "helper/RtspContext.h"
#include "helper/Event.h"
#include "live/Rtp.h"
#include "live/MediaSession.h"
#include "live/MediaSessionManager.h"

class RtspServer;


class RtspConnection {
	typedef void (*DisConnectCallback)(void *arg,int);
	typedef void (*SessionAddCallback)(void *th, TrackId track_id, RtpConnection* rtp_connection, const std::string& session_name);
	typedef void (*SessionRemoveCallback)(void *th, TrackId track_id, RtpConnection* rtp_connection, const std::string& session_name);
public:
	RtspConnection(RtspContext * ctx, int client_fd, RtspServer *server);
	~RtspConnection();

	void setDisconnectCallback(DisConnectCallback cb) { disconnect_cb_ = cb; }
	void setSessionAddCallback(SessionAddCallback cb, void* th) {
		session_add_cb = cb;
		rtsp_server = th;
	}
	void setSessionRemoveCallback(SessionRemoveCallback cb, void* th) {
		session_remove_cb = cb;
		rtsp_server = th;
	}
	void setRtspCloseCb(std::function<void(int client_fd)> cb) {
		RtspCloseCb = cb;
	}

	void setMediaSessionManagerPtr(void* media_session_manager_ptr) {
		media_session_manager_ptr_ = media_session_manager_ptr;
		LookMediaSession = std::bind(&MediaSessionManager::LookMediaSession, *((MediaSessionManager*)media_session_manager_ptr_), std::placeholders::_1);

	}

private:
	
	

	SessionAddCallback session_add_cb;
	SessionRemoveCallback session_remove_cb;

	void *media_session_manager_ptr_;
	void *rtsp_server;

	std::vector<std::shared_ptr<RtpConnection>> rtp_conns_;

	std::shared_ptr<IOEvent> io_event_;

	static void readCallback(void* conn);

	int handleRtspRequest();

	bool parseRequestLine(const char* line);
	bool parseRequestMethod(const char* line);

	RtspContext * ctx_;

	// this function is used for single thread
	bool handleOptions();
	bool handleDescribe();
	bool handleSetup();
	bool handlePlay();
	bool handleTeardown();

	DisConnectCallback disconnect_cb_{nullptr};
	

	std::string rbuf_;

	int buf_size_{ 1024 };

	int client_fd_{ 0 };
	RtspServer* server_;

	bool alive_{ false };

	// for temporary use
	int cseq_{ 0 };
	enum class Method {
		OPTIONS,
		DESCRIBE,
		SETUP,
		PLAY,
		TEARDOWN,
		UNKNOWN
	} method_;

	uint16_t port_;
	char ip_[24];
	char suffix_[128];
	char method_buf_[32];
	char url_[256];
	char version_[32];
	char result_[1024];
	std::string session_name_;
	session_id_t session_id_{ 0 };

	std::function<std::shared_ptr<MediaSession>(const std::string& session_name)>LookMediaSession;
	std::function<void(int client_fd)> RtspCloseCb;
};