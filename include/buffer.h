#ifndef BUFFER_H
#define BUFFER_H

#include <cstddef>
#include <vector>

// 定义缓冲区类
class Buffer {
    private:
        // 用于存放packet指针及对应引用计数的结构体
        struct PacketWrapper {
            char* packet;
            int refCount;
        };

        std::vector<PacketWrapper> packets; // 存放packet包装对象的向量
        size_t bufferSize;   // 缓冲区最大大小
        int maxPacketNum;    // 最大packet包装数量
        int packetNum;       // 当前存放的packet包装数量
        size_t totalSize;    // 当前存放packet的总大小
    
    public:
        // 构造函数
        Buffer(size_t size);

        // 析构函数
        ~Buffer();

        // 添加packet指针，并初始化引用计数为1
        bool addPacket(char* packet, size_t size);

        // 删除packet指针或减少其引用计数
        void removePacket(char* packet, size_t size);

        // 获取头部packet指针
        char* getPacket();
       
        // 缓冲区满？
        bool isBufferFull() const {
            return packetNum >= maxPacketNum;
        }

        // 缓冲区空？
        bool isBufferEmpty() const {
            return packets.empty();
        }

        // 获取当前packet数量
        int getPacketNum() const {
            return packetNum;
        }
};

#endif  // BUFFER_H