#pragma once

class RstpServer
{

public:


	RstpServer();
	~RstpServer();
	

private:

	const char* port_ = "11451";


	int socket_fd_;
};