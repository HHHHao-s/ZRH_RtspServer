#pragma once
#include <memory>
#include "helper/RtspContext.h"
class RtspConnection {

public:
	RtspConnection(std::shared_ptr<RtspContext> ctx, int client_fd);
	~RtspConnection();

	
	void run();


private:
	std::shared_ptr<RtspContext> ctx_;

	// this function is used for single thread
	int handleOptions();
	int handleDescribe();
	int handleSetup();
	int handlePlay();
	int handleTeardown();

	int handleRtspRequest();

	int playLoop();

	int client_fd_{ 0 };

	bool alive_{ false };

	// for temporary use
	int cseq_{ 0 };
	char method_[32];
	char url_[128];
	char version_[32];
};