# EchoServer 项目

## 项目简介
本项目实现了一个简单的回射服务器。通过封装 TCP 套接字和 packet 语义分割，实现了客户端与服务端之间基于完整数据包的通信。

## 设计说明

### 1. 套接字封装
- **TCPSocket**: 封装了基础的 socket 操作，包括发送、接收、非阻塞模式设置和异常处理。对发送和接收的错误情况（例如连接断开）进行了处理，当发生错误时会关闭连接并抛出异常。
- **TCPClient/TCPServer**: 分别用于客户端和服务端，其中 TCPServer 提供绑定、监听和接受连接的功能；TCPClient 提供连接和连接状态检查。

### 2. Packet 语义转换
- **PacketSocket**: 继承自 TCPSocket，负责将 TCP 流数据转换为 packet 数据。通过内部临时缓冲区累积从内核接收到的数据，并使用固定的包头格式（例如前4字节表示 packet 长度）解析完整的数据包。只有当接收到完整的 packet 后，才将内存地址传递给接收缓冲区供上层应用使用。

### 3. 缓冲区管理
- **Buffer**: 负责管理存储完整 packet 的内存地址。采用 vector 存储 packet 指针，提供添加、删除以及获取头部 packet 的接口。用于在 PacketSocket 中管理接收和发送的数据包。

## 项目结构
- **/include**: 各种头文件，如 tcpsocket.h、packetsocket.h、buffer.h 等。
- **/src**: 源代码实现，如 tcpsocket.cpp、packetsocket.cpp、buffer.cpp 等。
- **CMakeLists.txt**: CMake 构建脚本。

## 编译与运行
使用 CMake 构建项目：
```bash
mkdir build
cd build
cmake ..
make
./echoServer
```

## 注意事项
- 通过 Buffer 类管理完整的 packet 数据，保证 TCP 流数据能组装成符合应用层语义的完整报文。
- 项目中通过异常机制处理对端断开和其他 TCP 错误，确保程序健壮性。

## License
本项目遵循 MIT 协议，详细内容请参阅项目根目录下的 [LICENSE](LICENSE) 文件。
