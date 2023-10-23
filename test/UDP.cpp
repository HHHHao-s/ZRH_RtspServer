#include <stdio.h>

#include <sys/socket.h>
#include <arpa/inet.h> // sockaddr_in, inet_addr
#include <unistd.h>    // close
#include <cstring>     

#include <errno.h>

const char* SRV_ADDR = "127.0.0.1";
int SRV_PORT = 8096;

int main()
{
    /// 1、创建socket
    int socket_fd = ::socket(AF_INET, SOCK_DGRAM, 0); // udp
    if (socket_fd == -1) {
        printf("%s: create socket failed. %s\n", __func__, strerror(errno));
        return 1;
    }
    else {
        printf("%s: create socket (fd = %d) success.\n", __func__, socket_fd);
    }

    /// 2、绑定到本地端口
    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;                                 // IPv4
    //servaddr.sin_addr.s_addr = inet_addr(SRV_ADDR);              //仅支持IPv4
    inet_pton(servaddr.sin_family, SRV_ADDR, &servaddr.sin_addr);  // 新函数，通用IPv4/6
    servaddr.sin_port = htons(SRV_PORT);

    int ret = ::bind(socket_fd, (const sockaddr*)&servaddr, sizeof(servaddr));
    if (ret == -1) {
        printf("%s: bind %s:%d failed. %s \n", __func__, SRV_ADDR, SRV_PORT, strerror(errno));
        return 1;
    }
    else {
        printf("%s: bind %s:%d success.\n", __func__, SRV_ADDR, SRV_PORT);
    }

    /// 3、等待接收和响应
    char buf[1024]; // 发送

    sockaddr_in clientaddr;
    socklen_t   socklen = sizeof(clientaddr);

    while (1)
    {
        // 接收客户端数据
        int len = ::recvfrom(socket_fd, buf, sizeof(buf), 0, (struct sockaddr*)&clientaddr, &socklen);

        if (len < 0) {
            printf("%s: recv failed. err %s \n", __func__, strerror(errno));
            break;
        }
        else {
            // 获取客户端的ip、和port
            char ip[INET6_ADDRSTRLEN];
            inet_ntop(clientaddr.sin_family, &clientaddr.sin_addr, ip, socklen);

            int port = ntohs(clientaddr.sin_port);

            printf("%s: client [%s:%d] recv %2d: %s\n", __func__, ip, port, len, buf);
        }

        // 接收到的数据发送给客户端
        len = ::sendto(socket_fd, buf, sizeof(buf), 0, (struct sockaddr*)&clientaddr, socklen);
        if (len < 0) {
            printf("%s: send failed. err %s \n", __func__, strerror(errno));
            break;
        }
    }

    /// 4、关闭连接
    ::close(socket_fd);
}
