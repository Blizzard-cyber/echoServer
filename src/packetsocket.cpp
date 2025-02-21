#include "packetsocket.h"
#include "global.h"
#include <cstring>
#include <iostream>

PacketSocket::PacketSocket(int sockfd, size_t bufSize)
    : TCPSocket(sockfd), 
      recvBuffer(bufSize), sendBuffer(bufSize),
      tempBufferSize(bufSize), tempDataLen(0)
{
    tempBuffer = new char[tempBufferSize];
}

PacketSocket::~PacketSocket() {
    delete [] tempBuffer;
}

// 解析报文头部，获取报文长度，len为当前接收到的数据长度（接受到完整packet时返回packet长度，否则返回0）
size_t PacketSocket::parsePacket(const char* data, size_t len) {
    if(len < HEADERLEN) return 0;
    uint32_t packetLen = 0;
    std::memcpy(&packetLen, data, HEADERLEN);
    packetLen = ntohl(packetLen);
    if(len >= packetLen + HEADERLEN) {
        return packetLen + HEADERLEN;
    }
    return 0;
}

// 接收packet
bool PacketSocket::recvPacket() {
    ssize_t recvLen = Recv(tempBuffer + tempDataLen, tempBufferSize - tempDataLen);
    if(recvLen <= 0) {
        return false;
    }
    tempDataLen += recvLen;
    size_t packetSize = parsePacket(tempBuffer, tempDataLen);
    while(packetSize > 0) {
        // 将当前tempBuffer的地址直接传给接收缓冲区，构成一个完整packet
        recvBuffer.addPacket(tempBuffer, packetSize);
        
        // 为接收区的剩余数据重新申请一个内存块
        size_t remainingSize = tempDataLen - packetSize;
        char* newBuffer = new char[tempBufferSize];
        if(remainingSize > 0) {
            memcpy(newBuffer, tempBuffer + packetSize, remainingSize);
        }
        // 此时tempBuffer中存放的完整packet已交由Buffer管理，不再释放
        tempBuffer = newBuffer;
        tempDataLen = remainingSize;
        packetSize = parsePacket(tempBuffer, tempDataLen);
    }
    return true;
}

void PacketSocket::queuePacket(char* packet, size_t size) {
    sendBuffer.addPacket(packet, size);
}

bool PacketSocket::sendPacket() {
    if(sendBuffer.isBufferEmpty())
        return false;
    char* packet = sendBuffer.getPacket(); // 获取发送缓冲区的头部packet
    uint32_t packetLen = 0;
    memcpy(&packetLen, packet, sizeof(packetLen));
    packetLen = ntohl(packetLen);
    size_t sendSize = packetLen + HEADERLEN;
    
    ssize_t sent = Send(packet, sendSize);
    if(sent == (ssize_t)sendSize) {
        sendBuffer.removePacket(packet, sendSize);
        delete [] packet;
        return true;
    } else {
        return false;
    }
}
