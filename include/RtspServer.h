#pragma once

class RtspServer
{

public:


	RtspServer();
	~RtspServer();
	void Start();
	

private:

	char port_[6] = "11451";


	int socket_fd_;
};