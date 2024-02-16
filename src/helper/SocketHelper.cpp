#include "helper/SocketHelper.h"
#include <arpa/inet.h>

#ifndef _WIN32
int Close(int fd) {
	if (close(fd) < 0) {
		LOG_ERROR("close error\n");
		return -1;
	}
	return 0;
}

int Setsockopt(int fd, int level, int optname, const void* optval, socklen_t optlen) {
	if (setsockopt(fd, level, optname, optval, optlen) < 0) {
		LOG_ERROR("setsockopt error\n");
		return -1;
	}
	return 0;
}

int Freeaddrinfo(struct addrinfo* listp) {
	freeaddrinfo(listp);
	return 0;
}

int Getaddrinfo(const char* node, const char* service, const struct addrinfo* hints, struct addrinfo** result) {
	int ret = 0;
	if ((ret = getaddrinfo(node, service, hints, result)) != 0) {
		LOG_ERROR("getaddrinfo error\n");
		return -1;
	}
	return ret;
}

int Accept(int listenfd, struct sockaddr* addr, socklen_t* addrlen) {
	int connfd;
	if ((connfd = accept(listenfd, addr, addrlen)) < 0) {
		LOG_ERROR("accept error\n");
		return -1;
	}
	return connfd;
}

int OpenListenfd(const char* port) {
	struct addrinfo hints, * listp, * p;
	int listenfd, optval = 1;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV;
	if (getaddrinfo(NULL, port, &hints, &listp) != 0) {
		LOG_ERROR("getaddrinfo error\n");
		return -1;
	}
	for (p = listp; p; p = p->ai_next) {

		if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
			LOG_ERROR("socket error\n");

			continue;
		}

		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));

		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
			break;
		}
		Close(listenfd);

	}

	Freeaddrinfo(listp);

	if (listen(listenfd, 1024) < 0) {
		LOG_ERROR("listen error\n");
		Close(listenfd);
		return -1;
	}

	return listenfd;

}

int OpenAcquireClientUdp(uint16_t accquire_listen_port) {

	char buf[64] = { 0 };

	
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		LOG_ERROR("socket error\n");
		return -1;
	}

	sprintf(buf, "%d", accquire_listen_port);
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(accquire_listen_port);


	if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		LOG_ERROR("bind failed");
		Close(sockfd);
		exit(EXIT_FAILURE);
	}

	LOG_INFO("listen_port:%d\n",(int)accquire_listen_port);
	return sockfd;
}

int OpenClientUdp(uint16_t *listen_port) {
	
	char buf[64] = { 0 };

	while (1) {
		int random_port = rand() % (PORT_END - PORT_START + 1) + PORT_START;
		sprintf(buf, "netstat -an | grep :%d > /dev/null", random_port);
		if(system(buf))
		{
			*listen_port = random_port;
			break;
		}
	}
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		LOG_ERROR("socket error\n");
		return -1;
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(*listen_port);

	
	if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		LOG_ERROR("bind failed");
		Close(sockfd);
		exit(EXIT_FAILURE);
	}
	LOG_INFO("listen_port:%d\n", (int)*listen_port);

	return sockfd;
}

int Write(int fd, const void* buf, size_t count) {

	int n = 0;
	if ((n = write(fd, buf, count)) < 0) {
		LOG_ERROR("write error\n");

	}
	return n;

}

int SendTo(int fd, const void* buf, size_t count, int flags, const struct sockaddr* addr, socklen_t addrlen) {
	int n = 0;
	if ((n = sendto(fd, buf, count, flags, addr, addrlen)) < 0) {
		LOG_ERROR("sendto error\n");

	}
	return n;
	
}

int Getsockname(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
	int n = 0;
	if ((n = getsockname(sockfd, addr, addrlen)) < 0) {
		LOG_ERROR("getsockname error\n");

	}
	return n;
}

int GetPeerName(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
	int n = 0;
	if ((n = getpeername(sockfd, addr, addrlen)) < 0) {
		LOG_ERROR("getpeername error\n");

	}
	return n;
}

void ShowAddress(sockaddr_in* sock_addr) {
	char ip[64] = { 0 };
	inet_ntop(AF_INET, &sock_addr->sin_addr, ip, sizeof(ip));
	LOG_INFO("ip:%s port:%d\n", ip, ntohs(sock_addr->sin_port));
}

#else



#endif // !_WIN32

