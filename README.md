# echoServer

## 项目简介
该项目实现了一个基于 TCP 和 epoll 的回声服务器及客户端。服务器使用单线程 Epool 驱动处理高并发连接，并通过自定义数据包协议实现数据的回射处理。详细的设计、性能测试及优化分析详见 [技术报告](./report.md)。

## 设计说明

### 1. 套接字及数据包封装
- **TCPSocket**：封装基础 socket 操作，包括发送、接收和错误处理。  
- **TCPClient/TCPServer**：分别用于客户端和服务端，两者均支持基于 epoll 的 IO 多路复用。  
- **PacketSocket**：在 TCP 流和应用层 Packet 之间做转换，实现数据打包和解析。

### 2. 事件与缓冲区管理
- **Epoller**：单线程采用 epoll 事件循环，高效管理成千上万 socket 的读写事件。  
- **Buffer**：管理接收和发送缓冲区，利用引用计数减少数据拷贝开销。

### 3. 报告引用
详细设计思路、系统架构图、时序图、性能测试报告及优化分析均记录在 [技术报告](./report.md)。

## 目录结构
```
echoServer
├── CMakeLists.txt
├── include
│   ├── buffer.h
│   ├── epoller.h
│   ├── global.h
│   ├── packet.h
│   ├── packetsocket.h
│   └── tcpsocket.h
├── src
│   ├── buffer.cpp
│   ├── epoller.cpp
│   ├── packetsocket.cpp
│   ├── tcpsocket.cpp
│   └── ...
├── client.cpp
├── server.cpp
├── README.md
└── report.md
```

## 编译方法
```bash
mkdir build
cd build
cmake ..
make
```

## 运行方法
1. 启动服务器：
   ```bash
   ./server
   ```
2. 启动客户端：
   ```bash
   ./client
   ```

## 项目配置
- 服务器监听端口：8080（可在 `global.h` 中调整）。  
- 报头长度：8 字节（4 字节类型 + 4 字节长度）。  
- 均采用基于 epoll 的 IO 多路复用方案。
