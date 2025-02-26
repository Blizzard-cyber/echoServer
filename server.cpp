#include <iostream>
#include <unordered_map>
#include "tcpsocket.h"
#include "packetsocket.h"
#include "epoller.h"
#include "global.h"      
#include "error.h"       
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    try {
        // 创建服务器并绑定监听端口
        TCPServer server;
        if (!server.Bind(SERVER_PORT, "0.0.0.0")) {
            std::cerr << "Bind failed" << std::endl;
            return 1;
        }
        if (!server.Listen(LISTENQ)) {
            std::cerr << "Listen failed" << std::endl;
            return 1;
        }
        std::cout << "Server started, listening on port: " << SERVER_PORT << std::endl;
        
        // 创建 Epoller，假设最大文件描述符数为1024
        Epoller epoller(0, 1024);
        // 将服务器套接字添加到 epoll 中监控可读事件
        epoller.addfd(server.getSocketFD(), EPOLLIN, true);
        
        // 用于管理客户端连接，将套接字fd与 PacketSocket 对象关联（注意：这里只做示例，内存管理需要完善）
        std::unordered_map<int, PacketSocket*> clientMap;
        
        while (true) {
            int nReady = epoller.wait(-1);
            for (int i = 0; i < nReady; ++i) {
                int fd = epoller.getEventOccurfd(i);  // 获取事件发生的文件描述符
                uint32_t events = epoller.getEvents(i);
                // 如果事件发生在监听套接字上，则处理新连接
                if (fd == server.getSocketFD()) {
                    // 调用封装后的 Accept 函数
                    int connfd = server.Accept();
                    //if (connfd != -1) {  // Check if connection is valid
                        // 使用 PacketSocket 封装客户端 socket，缓冲区大小为1024字节
                        PacketSocket* ps = new PacketSocket(connfd, 1024);
                        clientMap[connfd] = ps;
                        // 将新连接添加到 epoll 监听中，采用水平触发模式
                        epoller.addfd(connfd, EPOLLIN, false);
                        std::cout << "New client connected: " << connfd << std::endl;
                    //}
                } else {
                    // 客户端套接字有事件
                    auto it = clientMap.find(fd);
                    if (it != clientMap.end()) {
                        PacketSocket* ps = it->second;
                        if (events & EPOLLIN) {
                            // 尝试接收数据
                            if (ps->recvPacket()) {
                                // 成功接收到完整数据包，接下来发送数据包
                                if(ps->sendPacket()) {
                                    std::cout << "Echo packet sent." << std::endl;
                                } else {
                                    std::cerr << "Failed to send echo packet." << std::endl;
                                }
                                
                            }
                        }
                        else if (events & EPOLLERR) {
                            // 错误事件处理
                            std::cerr << "Error event on client: " << fd << std::endl;
                            epoller.delfd(fd);
                            delete ps;
                            clientMap.erase(it);
                        }
                    } else {
                        std::cerr << "Unknown client fd: " << fd << std::endl;
                    }
                }
            }
        }
        
        // 清理客户端连接
        for (auto& pair : clientMap) {
            delete pair.second;
        }
    } catch (const EpollException& ex) {
        std::cerr << "Epoll exception: " << ex.what() << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }
    return 0;
}
