#pragma once
class RtspConnection {

public:
	RtspConnection(int client_fd);
	~RtspConnection();

	
	void run();


private:
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