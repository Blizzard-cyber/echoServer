#ifndef GLOBAL_H
#define GLOBAL_H

extern const int MAX_EVENTS;    // epoll 最大事件数
extern const int CLIENT_NUM;    // 客户端（压力发生器数）
extern const int HEADERLEN;     // 包头长度

extern const int SERVER_PORT;   // 服务器监听端口
extern const char* SERVER_IP;   // 服务器监听地址
extern const int MAX_CONN;      // 服务器最大连接数
extern const int LISTENQ;       // 服务器监听队列长度

#endif // GLOBAL_H