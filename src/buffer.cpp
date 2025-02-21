#include "buffer.h"
#include <algorithm>
#include <iostream>


Buffer::Buffer(size_t size)
    : bufferSize(size), maxPacketNum(size / sizeof(char*)), packetNum(0), totalSize(0){}

Buffer::~Buffer() {
    packets.clear();
}

void Buffer::addPacket(char* packet, size_t size) {
    if(packetNum < maxPacketNum) {
        packets.push_back(packet);
        packetNum++;
        totalSize += size;
    }
    else {
        // 缓冲区已满
        std::cout << "Buffer is full." << std::endl;
    }
}

void Buffer::removePacket(char* packet, size_t size) {
    if(packets.empty()) {
        // 缓冲区为空
        std::cout << "Buffer is empty." << std::endl;
        return;
    }
    auto it = std::find(packets.begin(), packets.end(), packet);
    if(it != packets.end()) {
        packets.erase(it);
        packetNum--;
        totalSize -= size;
    }
    else {
        // 未找到指定 packet
        std::cout << "Packet not found." << std::endl;
    }
}


char* Buffer::getPacket() {
    if (packets.empty()) {
        std::cout << "Buffer is empty." << std::endl;
        return nullptr;
    }
    return packets.front();
}