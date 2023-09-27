#pragma once
#include "helper/LOG.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <memory.h>

int OpenListenfd(char* port);
int Close(int fd);
int Freeaddrinfo(struct addrinfo* listp);
int Getaddrinfo(const char* hostname, const char* port, const struct addrinfo* hints, struct addrinfo** result);
int Setsockopt(int s, int level, int optname, const void* optval, socklen_t optlen);
int Accept(int s, struct sockaddr* addr, socklen_t* addrlen);