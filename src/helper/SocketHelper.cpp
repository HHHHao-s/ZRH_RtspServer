#include "helper/SocketHelper.h"

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
	if ((ret = getaddrinfo(node, service, hints, result))!= 0) {
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

int OpenListenfd(char* port) {
	struct addrinfo hints, *listp, *p;
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