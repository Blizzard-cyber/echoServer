#ifndef BUFFER_H
#define BUFFER_H

#include <cstddef>
#include <vector>

// 定义缓冲区类
class Buffer {
    private:
        std::vector<char*> packets; // 存放指向packet的指针的向量
        size_t bufferSize;   // 缓冲区最大大小
        int maxPacketNum;   // 最大packet指针数量
        int packetNum;  // 现存放packet指针的数量
        size_t totalSize;   // 现存放packet的总大小
    
    public:
        // 构造函数
        Buffer(size_t size);

        // 析构函数
        ~Buffer();

        // 添加指向packet的指针
        void addPacket(char* packet, size_t size);

        // 删除指向packet的指针
        void removePacket(char* packet, size_t size);

        // 获取头部packet指针
        char* getPacket() ;
       

        // 缓冲区满？
        bool isBufferFull() const {
            return packetNum >= maxPacketNum;
        }

        // 缓冲区空？
        bool isBufferEmpty() const {
            return packets.empty();
        }
};

#endif  // BUFFER_H