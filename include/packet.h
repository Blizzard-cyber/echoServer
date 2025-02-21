#ifndef PACKET_H
#define PACKET_H

#include <cstdint>
#include <vector>

struct Packet {
    // 报文长度，单位为字节
    uint32_t length;
    // 报文内容
    std::vector<uint8_t> content;

    // 默认构造函数
    Packet() : length(0), content() {}

    // 根据内容构造 Packet
    Packet(const std::vector<uint8_t>& data)
        : length(static_cast<uint32_t>(data.size())), content(data) {}

    // 根据内容构造 Packet，提供 std::vector<char> 支持
    Packet(const std::vector<char>& data)
        : length(static_cast<uint32_t>(data.size())), content(data.begin(), data.end()) {}
    

    void fillContent(uint32_t size) {
        length = size;
        content.resize(size);
    }


    uint8_t* getContentPointer() {
        return content.data();
    }
};

#endif // PACKET_H
