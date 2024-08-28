# RTSP流媒体服务器


## 项目结构

![structure](structure.jpg)


使用epoll作为timer和socket的事件驱动，实现了一个简单的RTSP流媒体服务器。

timer用于控制RTP包的发送频率，socket用于接收RTSP请求

当前实现了OPTIONS、DESCRIBE、SETUP、PLAY、TEARDOWN五个RTSP请求

收到rtsp请求时，会维持一个rtsp连接以及新建一个rtp连接，根据客户端连接方式，rtp连接可以是tcp或者udp

tcp连接时，rtp包会通过tcp发送，并且用客户端传递的channel id来区分rtp和rtcp

udp连接时，rtp和rtcp会通过发送给客户端的指定端口，并且将在服务器端开启的udp端口发送给客户端

需要发送rtp包时，会调用发送的回调函数进行发送


## 构建

```shell
mkdir build
cd build
cmake ..
make
```

## 运行

```shell
./RstpServer
```

## FFplay播放

tcp模式播放

```shell
ffplay -i rtsp://127.0.0.1:11451/test -rtsp_transport tcp
```

udp模式播放

```shell
ffplay -i rtsp://127.0.0.1:11451/test
```