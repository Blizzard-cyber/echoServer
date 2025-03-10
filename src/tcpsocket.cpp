#include "tcpsocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>
#include <cstring>
#include <stdexcept>
#include <errno.h>

// ===================== TCPSocket 实现 =====================
TCPSocket::TCPSocket(int sockfd) 
    : m_sockfd(sockfd), m_non_blocking(false) {}

TCPSocket::~TCPSocket() {
    Close();
}


void TCPSocket::SetNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);  
    if (flags == -1) throw std::runtime_error("fcntl(F_GETFL) failed");  

    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);  
    m_non_blocking = true;  
}

ssize_t TCPSocket::Send(const char* buf, size_t len, int flags) {
    ssize_t ret = send(m_sockfd, buf, len, flags);
    if(ret == -1) {
        // 处理对端断开或其他错误
        if(errno == EPIPE || errno == ECONNRESET) {
            
            //Close();
            throw std::runtime_error("Send failed: connection closed");
        }
        throw std::runtime_error(std::string("Send failed: ") + strerror(errno));
    }
    return ret;
}

ssize_t TCPSocket::Recv(char* buf, size_t len, int flags) {
    ssize_t ret = recv(m_sockfd, buf, len, flags);
    if(ret == 0) {
        // 对端关闭连接
        Close();
        throw std::runtime_error("Recv failed: connection closed by peer");
    }
    if(ret == -1) {
        throw std::runtime_error(std::string("Recv failed: ") + strerror(errno));
    }
    return ret;
}

void TCPSocket::Close() {
    if (m_sockfd != -1) {
        close(m_sockfd);
        m_sockfd = -1;
    }
}

// ===================== TCPClient 实现 =====================
TCPClient::TCPClient() : TCPSocket(-1) {}

bool TCPClient::Connect(const std::string& ip, uint16_t port, bool non_block) {
    Close();  // 清理现有连接

    struct addrinfo hints, *result, *rp;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    std::string port_str = std::to_string(port);
    int ret = getaddrinfo(ip.c_str(), port_str.c_str(), &hints, &result);
    if (ret != 0) return false;

    // 遍历所有地址尝试连接
    for (rp = result; rp != nullptr; rp = rp->ai_next) {
        m_sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (m_sockfd == -1) continue;

        SetNonBlocking(m_sockfd);  // 设置非阻塞模式

        if (connect(m_sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            freeaddrinfo(result);
            return true;  // 立即连接成功
        }

        if (errno == EINPROGRESS) {
            freeaddrinfo(result);
            return true;  // 非阻塞模式下正在连接
        }

        Close();  // 当前地址失败，尝试下一个
    }

    freeaddrinfo(result);
    return false;
}

bool TCPClient::CheckConnectStatus(int timeout_ms) {
    if (m_sockfd == -1) return false;

    struct pollfd pfd;
    pfd.fd = m_sockfd;
    pfd.events = POLLOUT;

    // 等待套接字可写
    int ret = poll(&pfd, 1, timeout_ms);
    if (ret <= 0) return false;

    // 检查实际连接状态
    int so_error = 0;
    socklen_t len = sizeof(so_error);
    getsockopt(m_sockfd, SOL_SOCKET, SO_ERROR, &so_error, &len);
    return (so_error == 0);
}

// ===================== TCPServer 实现 =====================
TCPServer::TCPServer() {
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd == -1) {
        throw std::runtime_error("Socket creation failed");
    }

    // 设置为非阻塞模式
    SetNonBlocking(m_sockfd);

    // 设置地址重用选项
    int yes = 1;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        Close();
        throw std::runtime_error("Set SO_REUSEADDR failed");
    }
}

bool TCPServer::Bind(uint16_t port, const std::string& ip) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
        return false;
    }
    
    return bind(m_sockfd, (struct sockaddr*)&addr, sizeof(addr)) == 0;
}

bool TCPServer::Listen(int backlog) {
    return listen(m_sockfd, backlog) == 0;
}

// TCPClient* TCPServer::Accept() {
//     struct sockaddr_in client_addr;
//     socklen_t addr_len = sizeof(client_addr);
    
//     int client_fd = accept(m_sockfd, (struct sockaddr*)&client_addr, &addr_len);
//     if (client_fd == -1) return nullptr;

//     // 创建客户端对象并转移socket所有权
//     TCPClient* client = new TCPClient();
//     client->Close();  // 确保清理原有描述符
//     client->m_sockfd = client_fd;
//     client->SetNonBlocking(true);  // 客户端默认非阻塞
//     return client;
// }

int TCPServer::Accept() {
    int connfd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    connfd = accept(m_sockfd, (struct sockaddr*)&client_addr, &addr_len);
    if (connfd < 0) {
        if (errno == EINTR || errno == EWOULDBLOCK)
            return -1;
        else
            throw std::runtime_error("accept error");
    }
    //设置为非阻塞模式
    SetNonBlocking(connfd);
    
    return connfd;

}