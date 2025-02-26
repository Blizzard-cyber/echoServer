#include "packet.h"
#include <cstring>
#include <cstdlib>



// 默认构造函数
Packet::Packet() {
    header.type = PacketHeader::PT_DATA_SEND; // 默认设为发送数据类型
    header.length = 0;
    data = nullptr;
}

// 带参构造函数
Packet::Packet(PacketHeader::PacketType type, uint32_t length, const char* d) {
    header.type = type;
    header.length = length;
    if(length > 0 && d) {
        data = new char[length];
        std::memcpy(data, d, length);
    } else {
        data = nullptr;
    }
}

// 拷贝构造函数
Packet::Packet(const Packet& pkt) {
    header = pkt.header;
    if(header.length > 0 && pkt.data) {
        data = new char[header.length];
        std::memcpy(data, pkt.data, header.length);
    } else {
        data = nullptr;
    }
}

// 赋值运算符
Packet& Packet::operator=(const Packet& pkt) {
    if(this != &pkt) {
        delete [] data;
        header = pkt.header;
        if(header.length > 0 && pkt.data) {
            data = new char[header.length];
            std::memcpy(data, pkt.data, header.length);
        } else {
            data = nullptr;
        }
    }
    return *this;
}

// 析构函数
Packet::~Packet() {
    delete [] data;
}

PacketHeader::PacketType Packet::getType() const {
    return header.type;
}

uint32_t Packet::getLength() const {
    return header.length;
}

const char* Packet::getData() const {
    return data;
}

void Packet::setType(PacketHeader::PacketType type) {
    header.type = type;
}

void Packet::setLength(uint32_t length) {
    header.length = length;
    // 注意：此处不对 data 内存进行重新分配，需配合 setData 使用
}

void Packet::setData(const char* d, uint32_t length) {
    delete [] data;
    header.length = length;
    if(length > 0 && d) {
        data = new char[length];
        std::memcpy(data, d, length);
    } else {
        data = nullptr;
    }
}

void Packet::clear() {
    delete [] data;
    data = nullptr;
    header.length = 0;
    // header.type 保持不变，可根据需求修改
}
