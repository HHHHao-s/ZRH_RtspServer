#pragma once
#include "helper/LOG.h"
#ifndef _WIN32 

#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <memory.h>
#include <random>
#define PORT_END 65535
#define PORT_START 49152
int OpenListenfd(const char* port);
int Close(int fd);
int Freeaddrinfo(struct addrinfo* listp);
int Getaddrinfo(const char* hostname, const char* port, const struct addrinfo* hints, struct addrinfo** result);
int Setsockopt(int s, int level, int optname, const void* optval, socklen_t optlen);
int Accept(int s, struct sockaddr* addr, socklen_t* addrlen);
// 随机分配端口，并存储到listen_port中
int OpenClientUdp(uint16_t* listen_port);
int Write(int fd, const void* buf, size_t count);
int SendTo(int fd, const void* buf, size_t count, int flags, const struct sockaddr* addr, socklen_t addrlen);
int Getsockname(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
#else 
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

int OpenListenfd(const char* port);
int Close(int fd);
int Freeaddrinfo(struct addrinfo* listp);
int Getaddrinfo(const char* hostname, const char* port, const struct addrinfo* hints, struct addrinfo** result);
int Setsockopt(int s, int level, int optname, const void* optval, socklen_t optlen);
int Accept(int s, struct sockaddr* addr, socklen_t* addrlen);
int OpenListendUdp(const char* port);
int Write(int fd, const void* buf, size_t count);



#endif // !_WIN32

