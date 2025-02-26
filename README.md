# echoServer

## 项目简介
该项目实现了一个基于 TCP 和 epoll 的回声服务器以及客户端。  
- **服务器**：使用 `TCPServer` 监听客户端连接，采用 `Epoller` 管理所有套接字事件，并通过 `PacketSocket` 对数据包进行组装、发送和回显。  
- **客户端**：使用 `TCPClient` 与服务器连接，通过 `PacketSocket` 生产和发送数据包，同时接收服务器的回显数据包。

## 设计说明

### 1. 套接字封装
- **TCPSocket**: 封装了基础的 socket 操作，包括发送、接收、非阻塞模式设置和异常处理。对发送和接收的错误情况（例如连接断开）进行了处理，当发生错误时会关闭连接并抛出异常。
- **TCPClient/TCPServer**: 分别用于客户端和服务端，其中 TCPServer 提供绑定、监听和接受连接的功能；TCPClient 提供连接和连接状态检查。

### 2. Packet 语义转换
- **PacketSocket**: 继承自 TCPSocket，负责将 TCP 流数据转换为 packet 数据。通过内部临时缓冲区累积从内核接收到的数据，并使用固定的包头格式（例如前4字节表示 packet 长度）解析完整的数据包。只有当接收到完整的 packet 后，才将内存地址传递给接收缓冲区供上层应用使用。实现了应用层Packet语义和TCP流语义的分割。

### 3. 缓冲区管理
- **Buffer**: 负责管理存储完整 packet 的内存地址。采用 vector 存储 packet 指针，提供添加、删除以及获取头部 packet 的接口。用于在 PacketSocket 中管理接收和发送的数据包。


## 目录结构
```
/home/admin000/NDSL/echoServer
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
│   └── ...其他源文件
├── client.cpp
└── server.cpp
```

## 编译方法
使用 CMake 进行构建。在项目根目录下执行以下命令：
```bash
mkdir build
cd build
cmake ..
make
```
该过程会生成两个可执行程序：`server` 和 `client`。

## 运行方法
1. 先启动服务器：
   ```bash
   ./server
   ```
2. 再启动客户端：
   ```bash
   ./client
   ```

## 项目配置
- 服务器监听端口：`8080`（可在 `global.h` 中调整）。
- 报头长度：`8` 字节（4 字节类型 + 4 字节长度）。
- 客户端与服务器均采用基于 epoll 的 IO 多路复用方式。

## 注意事项
- 源文件中部分功能（如报文的解析、缓冲区管理）是简化实现，实际使用时可根据需求调整。  
- 请确保环境支持 C++11 及以上标准，并在 Linux 下编译。
