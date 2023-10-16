#pragma once
#include <memory>
#include "helper/RtspContext.h"
#include "helper/Event.h"
#include "live/Rtp.h"
#include "live/MediaSession.h"

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
		session_ptr_ = th;
	}
	void setSessionRemoveCallback(SessionRemoveCallback cb, void* th) {
		session_remove_cb = cb;
		session_ptr_ = th;
	}

private:
	
	SessionAddCallback session_add_cb;
	SessionRemoveCallback session_remove_cb;
	void *session_ptr_;

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
	std::string session_name_;
};