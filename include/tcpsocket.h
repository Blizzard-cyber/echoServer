#ifndef TCPSOCKET_H
#define TCPSOCKET_H


#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <stdexcept>

class TCPSocket {
public:
    explicit TCPSocket(int sockfd = -1);
    virtual ~TCPSocket();

    int m_sockfd;
    bool m_non_blocking;

    void SetNonBlocking(bool enable);

    // 基础IO操作
    ssize_t Send(const void* buf, size_t len, int flags = 0);
    ssize_t Recv(void* buf, size_t len, int flags = 0);
    
    // 连接管理
    virtual void Close();
    
    // 状态获取
    int GetFD() const { return m_sockfd; }
    bool IsNonBlocking() const { return m_non_blocking; }
};

class TCPClient : public TCPSocket {
public:
    TCPClient();
    
    // 非阻塞连接方法
    bool Connect(const std::string& ip, uint16_t port, bool non_block = true);
    
    // 连接状态检查
    bool CheckConnectStatus(int timeout_ms = 0);
};

class TCPServer : public TCPSocket {
public:
    TCPServer();
    
    // 绑定监听
    bool Bind(uint16_t port, const std::string& ip = "0.0.0.0");
    bool Listen(int backlog = SOMAXCONN);
    
    // 接受连接（返回客户端socket）
    TCPClient* Accept();
};
#endif // TCPSOCKET_H