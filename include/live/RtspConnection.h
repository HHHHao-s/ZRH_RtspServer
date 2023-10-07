#pragma once
#include <memory>
#include "helper/RtspContext.h"
#include "helper/Event.h"

class RtspServer;

class RtspConnection {
	typedef void (*DisConnectCallback)(void *arg,int);
public:
	RtspConnection(std::shared_ptr<RtspContext> ctx, int client_fd, RtspServer *server);
	~RtspConnection();

	void setDisconnectCallback(DisConnectCallback cb) { disconnect_cb_ = cb; }

private:
	

	static void readCallback(void* conn);

	int handleRtspRequest();

	std::shared_ptr<RtspContext> ctx_;

	// this function is used for single thread
	int handleOptions();
	int handleDescribe();
	int handleSetup();
	int handlePlay();
	int handleTeardown();

	DisConnectCallback disconnect_cb_{nullptr};
	


	int playLoop();

	int client_fd_{ 0 };
	RtspServer* server_;

	bool alive_{ false };

	// for temporary use
	int cseq_{ 0 };
	char method_[32];
	char url_[128];
	char version_[32];
};