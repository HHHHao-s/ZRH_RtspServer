
int OpenListenfd(char* port);
int Close(int fd);
int Freeaddrinfo(struct addrinfo* listp);
int Getaddrinfo(const char* hostname, const char* port, const struct addrinfo* hints, struct addrinfo** result);
int Setsockopt(int s, int level, int optname, const void* optval, socklen_t optlen);