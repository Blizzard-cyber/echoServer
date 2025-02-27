#ifndef PACKETSOCKET_H
#define PACKETSOCKET_H

#include "tcpsocket.h"
#include "buffer.h"
#include <cstddef>

// 封装 TCP 与 packet 语义分割的抽象类
class PacketSocket : public TCPSocket {
    protected:
        // 客户端使用两个缓冲区，服务端可只用一个回射缓冲区
        Buffer recvBuffer;  // 接收缓冲区：存放组装好的完整packet
        Buffer sendBuffer;  // 发送缓冲区：存放待发送packet指针
        
        // 内部临时缓冲区用于 TCP 流数据的累积
        char* tempBuffer;
        size_t tempBufferSize;
        size_t tempDataLen;
        
        // 基于协议解析完整 packet 的辅助函数，返回packet长度，若数据不完整返回0
        size_t parsePacket(const char* data, size_t len);
        
    public:
        PacketSocket(int sockfd, size_t bufSize,bool flag = false);
        virtual ~PacketSocket();
        
        // 从 TCP 内核缓冲区读取数据，组装完整packet到接收缓冲区
        bool recvPacket();
        
        // 从发送缓冲区取出packet并发送，发送成功后删除该packet指针
        bool sendPacket(Buffer& _buffer);
        
        // 将应用层构造好的packet添加到发送缓冲区
        void queuePacket(char* packet, size_t size);

        // // 用于服务器将发送和接收缓冲区设置为同一个
        // void setEchoMode() {
        //     sendBuffer = recvBuffer;
        // }
        
        // 获取接收缓冲区引用
        Buffer& getRecvBuffer() {
            return recvBuffer;
        }
        
        // 获取发送缓冲区引用
        Buffer& getSendBuffer() {
            return sendBuffer;
        }
};

#endif // PACKETSOCKET_H
