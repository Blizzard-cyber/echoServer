#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include "tcpsocket.h"
#include "packetsocket.h"
#include "epoller.h"
#include "global.h"   
#include "packet.h"   
#include <string>
#include <chrono>
#include <thread>
#include <random>


// 生成随机数据函数，返回随机长度的字符串数据
std::string generateRandomData() {
    // 使用系统时间作为种子
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine engine(seed);
    // 随机数据长度介于1到100之间
    std::uniform_int_distribution<int> lengthDist(1, 100);
    int len = lengthDist(engine);
    
    // 随机生成可打印字符范围的数据（ASCII 32 到 126）
    std::uniform_int_distribution<int> charDist(32, 126);
    std::string data;
    data.reserve(len);
    for (int i = 0; i < len; ++i) {
        char randomChar = static_cast<char>(charDist(engine));
        data.push_back(randomChar);
    }
    return data;
}

// 生产并打包一个packet，并将其加入发送缓冲区
void packPacket(PacketSocket& ps) {
    std::string data = generateRandomData();
    uint32_t dataLen = data.length();
    uint32_t netDataLen = htonl(dataLen);
    uint32_t type = htonl(PacketHeader::PT_DATA_SEND);
    size_t packetSize = HEADERLEN + dataLen;

    // 分配内存存放 packet（报头 + 数据）
    char* packet = new char[packetSize];
    std::memcpy(packet, &type, sizeof(uint32_t));
    std::memcpy(packet + sizeof(uint32_t), &netDataLen, sizeof(uint32_t));
    std::memcpy(packet + HEADERLEN, data.c_str(), dataLen);
    
    // 将 packet 地址加入发送缓冲区，然后调用 sendPacket 完整发送
    ps.queuePacket(packet, packetSize);
}

int main() {
    try {
        TCPClient client;
        if (!client.Connect(SERVER_IP, SERVER_PORT, true)) {
            std::cerr << "Connection failed!" << std::endl;
            return -1;
        }
        std::cout << "Connected to server." << std::endl;
        // 使用 PacketSocket 封装客户端 socket（缓冲区设为1024字节）
        PacketSocket ps(client.getSocketFD(), 1024);
        
        // 创建 epoller，添加客户端套接字同时监听读和写事件
        Epoller epoller(0, 1024);
        epoller.addfd(ps.getSocketFD(), EPOLLIN | EPOLLOUT, false);
        
        // 主循环：监听写就绪（发送数据）和读就绪（接收数据）
        while (true) {
            int nReady = epoller.wait(-1);
            for (int i = 0; i < nReady; ++i) {
                int fd = epoller.getEventOccurfd(i);
                uint32_t events = epoller.getEvents(i);
                if (fd == ps.getSocketFD()) {
                    // 写事件：生成数据、封装 packet 并发送
                    if (events & EPOLLOUT) {       
                        packPacket(ps);                      
                        if(!ps.sendPacket(ps.getSendBuffer())) {
                            std::cerr << "Failed to send packet." << std::endl;
                        } 
                    }
                    
                    // 读事件：接收数据，解析完整 packet 并输出处理结果
                    if (events & EPOLLIN) {
                        if(ps.recvPacket()) {
                            // 针对接收缓冲区中的每个 packet 进行处理
                            char* packet = nullptr;
                            while ((packet = ps.getRecvBuffer().getPacket()) != nullptr) {
                                uint32_t netType = 0, netLen = 0;
                                std::memcpy(&netType, packet, sizeof(uint32_t));
                                std::memcpy(&netLen, packet + sizeof(uint32_t), sizeof(uint32_t));
                                uint32_t type = ntohl(netType);
                                uint32_t dataLen = ntohl(netLen);
                                size_t fullSize = HEADERLEN + dataLen;
                                if (type != PacketHeader::PT_DATA_ECHO) {
                                    std::cerr << "Error: Received packet type: " << type << " is not PT_DATA_ECHO." << std::endl;
                                }
                                // 删除并释放该 packet 内存
                                ps.getRecvBuffer().removePacket(packet, fullSize);
                            }
                        }
                    }
                }
            }
        }
     } catch (const std::exception& ex) {
        std::cerr << "Client exception: " << ex.what() << std::endl;
    }
    return 0;
}
