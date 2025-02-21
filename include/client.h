#ifndef CLIENT_H_
#define CLIENT_H_
#include <arpa/inet.h>
#include <memory.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#include "buffer.h"


class Client {
 public:
  Client();
  Client(int server_port, const char* server_ip);
  virtual ~Client();

  int ConnectServer();  // 连接服务器
  ssize_t ReadData(int fd) ;  // 接受数据
  bool SendData(int fd) ;  // 发送数据
  void SetConnected(int& fd);  // 设置连接状态
  bool IsConnected() { return is_connected; }  // 是否已经连接

 protected:
  int server_port;  // 服务器端口
  const char* server_ip;  // 服务器ip
  bool is_connected = false;  // 是否已经连接
};

// // 压力发生器（线程）
// class PressureClient : public Client {
//  public:
//   PressureClient(int server_port, const char* server_ip, 
//                  int message_size, int max_test_time);
//   ~PressureClient();
//   ssize_t ReadData(int fd) override;
//   bool SendData(int fd) override;

//  private:
//   int message_size;           // 消息大小
//   int test_time;              // 测试次数
//   int max_test_time;          // 最大测试次数
// };

// // 接受回射数据（线程）
// class EchoServerClient : public Client {
//  public:
//   EchoServerClient(int server_port, const char* server_ip);
//   ~EchoServerClient();
//   ssize_t ReadData(int fd) override;
//   bool SendData(int fd) override;

//  private:
//   Buffer* buffer;     // 缓冲区
  
//};

#endif // CLIENT_H_