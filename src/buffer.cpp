#include "buffer.h"
#include <algorithm>
#include <iostream>
#include "global.h"

Buffer::Buffer(size_t size)
    : bufferSize(size), packetNum(0), totalSize(0) {
        maxPacketNum = bufferSize / HEADERLEN;  
}

Buffer::~Buffer() {
    // 这里如果需要释放 packet 内存，可以遍历 packets 并释放
    for (auto& pw : packets) {
        delete[] pw.packet;
    }
}

void Buffer::addPacket(char* packet, size_t size) {
    // 查找是否已存在相同 packet 指针
    auto it = std::find_if(packets.begin(), packets.end(),
        [packet](const PacketWrapper& pw) {
            return pw.packet == packet;
        });
    
    if (it != packets.end()) {
        // 如果存在，则引用计数增加，更新总大小
        it->refCount++;
        totalSize += size;
    } else {
        // 不存在则新建一个包装对象，引用计数初始化为1
        if (packetNum < maxPacketNum) {
            PacketWrapper pw = { packet, 1 };
            packets.push_back(pw);
            packetNum++;
            totalSize += size;
            
        } else {
            // 缓冲区已满，可根据实际需求抛出异常或做其他处理
            //std::cout << packetNum << std::endl;
            // return false; //添加失败
        
        }
    }
}

void Buffer::removePacket(char* packet, size_t size) {
    auto it = std::find_if(packets.begin(), packets.end(),
        [packet](const PacketWrapper& pw) {
            return pw.packet == packet;
        });
    
    if (it != packets.end()) {
        // 引用计数减少
        it->refCount--;
        totalSize = (totalSize >= size ? totalSize - size : 0);
        if (it->refCount <= 0) {
            // 引用计数为0则从 vector 中删除，并释放内存（如果需要）
            delete[] it->packet;  // 如果 packet 内存需要释放
            packets.erase(it);
            packetNum--;
        }
    } else {
        // 未找到指定 packet
        std::cout << "Packet not found." << std::endl;
    }
}

char* Buffer::getPacket() {
    if (!packets.empty()) {
        return packets.front().packet;
    }
    std::cout << "Buffer is empty." << std::endl;
    return nullptr;
}