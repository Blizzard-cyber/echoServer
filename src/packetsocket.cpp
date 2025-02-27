#include "packetsocket.h"
#include "global.h"
#include "packet.h"
#include <cstring>
#include <iostream>
#include <memory>

PacketSocket::PacketSocket(int sockfd, size_t bufSize,bool flag)
    : TCPSocket(sockfd), 
      recvBuffer(bufSize), sendBuffer(bufSize),
      tempBufferSize(bufSize), tempDataLen(0)
{
    tempBuffer = new char[tempBufferSize];
    if (!tempBuffer) {
        throw std::bad_alloc();  // 抛出异常，避免后续访问空指针
    }
}

PacketSocket::~PacketSocket() {
    delete [] tempBuffer;
}

// 修改后的 parsePacket 函数：假设 HEADERLEN 为 8（4字节类型 + 4字节长度）
size_t PacketSocket::parsePacket(const char* data, size_t len) {
    if(!data || len < HEADERLEN) return 0;
    // 跳过前4字节类型，从后4字节读取报文体长度
    uint32_t packetLen = 0;
    std::memcpy(&packetLen, data + sizeof(uint32_t), sizeof(uint32_t));
    packetLen = ntohl(packetLen);
    if(len >= packetLen + HEADERLEN) {
        return packetLen + HEADERLEN;
    }
    return 0;
}

// 修改后的 recvPacket 函数：使用循环多次调用 Recv，将所有完整 packet 分别分配新内存后存入 Buffer
bool PacketSocket::recvPacket() {
    bool gotPacket = false;
    //while (true) {
         ssize_t recvLen = Recv(tempBuffer + tempDataLen, tempBufferSize - tempDataLen);
         if(recvLen <= 0){
                return false;
         }
             
         tempDataLen += recvLen;
         size_t packetSize = parsePacket(tempBuffer, tempDataLen);
         while(packetSize > 0 && !recvBuffer.isBufferFull() ) {
            //   char* completePacket = new char[packetSize];
            //   std::memcpy(completePacket, tempBuffer, packetSize);
            //   recvBuffer.addPacket(completePacket, packetSize);
                char* completePacket = new char[packetSize];
                std::memcpy(completePacket, tempBuffer, packetSize);
                if (!recvBuffer.addPacket(completePacket, packetSize)) {
                    delete[] completePacket; // 确保释放
                }
                // recvBuffer.addPacket(completePacket, packetSize);
              gotPacket = true;
              size_t remainingSize = tempDataLen - packetSize;
              if(remainingSize > 0)
                   std::memmove(tempBuffer, tempBuffer + packetSize, remainingSize);
              tempDataLen = remainingSize;
              packetSize = parsePacket(tempBuffer, tempDataLen);
         }
    //}
    return gotPacket;
}

void PacketSocket::queuePacket(char* packet, size_t size) {
    if(!sendBuffer.addPacket(packet, size)) {
        delete[] packet;  // 如果添加失败则释放内存
    }
}

// 修改后的 sendPacket 函数：发送完整 packet 前更新packettype，若原类型为PT_DATA_SEND则改为PT_DATA_ECHO，再发送成功后从发送缓冲区中移除并释放内存
// 为了提升代码重用性，如果是服务端，传入的缓冲区地址即为发送缓冲区地址
bool PacketSocket::sendPacket(Buffer& _buffer) {
    if(_buffer.isBufferEmpty()){
        // std::cerr << "_buffer packet:"<< _buffer.getPacketNum() << std::endl;
        // std::cerr << "recvBuffer packet:"<< recvBuffer.getPacketNum() << std::endl;
            return false;
        }
    
    char* packet = _buffer.getPacket();
    if(!packet) {
        return false;
    }
    
    // 获取报文长度：从 header 的第二个4字节读取
    uint32_t packetLen = 0;
    std::memcpy(&packetLen, packet + sizeof(uint32_t), sizeof(uint32_t));
    packetLen = ntohl(packetLen);
    size_t sendSize = packetLen + HEADERLEN;
    
    // 如果为服务端回射，则修改 packet 中的type字段，从 PT_DATA_SEND 改为 PT_DATA_ECHO
    uint32_t type_net = 0;
    std::memcpy(&type_net, packet, sizeof(uint32_t));
    type_net = ntohl(type_net);
    if(type_net == PacketHeader::PT_DATA_SEND) {
         uint32_t echo_type = htonl(PacketHeader::PT_DATA_ECHO);
         std::memcpy(packet, &echo_type, sizeof(uint32_t));
    }
    
    size_t bytesSent = 0;
    while (bytesSent < sendSize) {
         ssize_t ret = Send(packet + bytesSent, sendSize - bytesSent);
         if(ret <= 0) {
             // 出现错误则退出循环
             break;
         }
         bytesSent += ret;
    }
    
    if(bytesSent == sendSize) {
        _buffer.removePacket(packet, sendSize);
        
        return true;
    }
    return false;
}
