#ifndef SERVER_H_
#define SERVER_H_

class Server {
 public:
  Server();
  Server(int MAX_CLIENTS, int server_port);
  virtual ~Server();

  int StartServer();  // 启动服务器

  // 接收客户端连接
  void AcceptClient(int listen_socket, int io_epoll_fd);

  // 回射操作
  void Echo(int io_epoll_fd);

protected:
    int MAX_CLIENTS;  // 最大客户端数
    int server_port;  // 服务器端口
    int server_fd;    // 服务器fd
    int client_fds[1024];  // 客户端fd
    int client_count = 0;  // 客户端数量
    bool is_running = false;  // 是否正在运行

};


#endif // SERVER_H_