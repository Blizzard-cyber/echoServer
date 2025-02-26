#ifndef PACKET_H
#define PACKET_H

#include <cstdint>
#include <vector>



//packet 头部
struct PacketHeader{
    //报文类型
    enum PacketType: uint32_t{
        PT_DATA_SEND = 0,  //发送数据
        PT_DATA_ECHO = 1,  //回射数据
        // PT_FIN_CLIENT = 2,   //客户端关闭连接
        // PT_FIN_SERVER = 3,   //服务器关闭连接
        // PT_HEARTBEAT = 4,    //心跳包
    };

    PacketType type;    //报文类型
    uint32_t length;    //报文长度
};


class Packet{
public:
    PacketHeader header;
    char* data;

    
    Packet();
    Packet(PacketHeader::PacketType type, uint32_t length, const char* data);
    Packet(const Packet& packet);
    Packet& operator=(const Packet& packet);
    ~Packet();

    PacketHeader::PacketType getType() const;
    uint32_t getLength() const;
    const char* getData() const;

    void setType(PacketHeader::PacketType type);
    void setLength(uint32_t length);
    void setData(const char* data, uint32_t length);

    void clear();

    // void serialize(std::vector<char>& buffer) const;
    // void deserialize(const std::vector<char>& buffer);

};
#endif // PACKET_H
