#ifndef Global_H
#define Global_H

const int MAX_EVENTS = 100;  //epoll 最大事件数
const int CLIENT_NUM = 10;   //客户端（压力发生器数）

const int HEADERLEN = 8;     //包头长度

//服务器监听端口
const int SERVER_PORT = 8080;
//服务器监听地址（环回地址）
const char* SERVER_IP = "127.0.0.1";
// 服务器最大连接数
const int MAX_CONN = 100;
// 服务器监听队列长度
const int LISTENQ = 20;

// 客户端单次生产数量
const int CLIENT_ONCE = 10;
#endif // Global_H