#include <iostream>
#include <vector>
#include <cstring>
#include <arpa/inet.h> // 用于字节序转换（Linux/macOS）
// 若为Windows，可替换为 <winsock2.h> 并使用 WSAHTONL/WSANTONL

// 包头类
class PacketHeader {
public:
    uint32_t magic = 0xDEADBEEF; // 魔数
    uint32_t length = 0;         // 包体长度
    uint32_t extra = 0;          // 预留字段

    // 序列化为字节流（返回固定长度的包头字节流）
    std::vector<char> serialize() const {
        std::vector<char> buffer(sizeof(PacketHeader));
        
        // 转换为网络字节序
        uint32_t net_magic = htonl(magic);
        uint32_t net_length = htonl(length);
        uint32_t net_extra = htonl(extra);

        // 按字段顺序写入字节流
        std::memcpy(buffer.data(), &net_magic, sizeof(net_magic));
        std::memcpy(buffer.data() + sizeof(net_magic), &net_length, sizeof(net_length));
        std::memcpy(buffer.data() + sizeof(net_magic) + sizeof(net_length), &net_extra, sizeof(net_extra));

        return buffer;
    }

    // 从字节流反序列化包头
    bool deserialize(const char* data) {
        // 从字节流中直接拷贝字段
        uint32_t net_magic, net_length, net_extra;
        std::memcpy(&net_magic, data, sizeof(net_magic));
        std::memcpy(&net_length, data + sizeof(net_magic), sizeof(net_length));
        std::memcpy(&net_extra, data + sizeof(net_magic) + sizeof(net_length), sizeof(net_extra));

        // 转换为主机字节序
        magic = ntohl(net_magic);
        length = ntohl(net_length);
        extra = ntohl(net_extra);

        return true;
    }

    // 打印包头信息（调试用）
    void print() const {
        std::cout << "[Header] magic=0x" << std::hex << magic 
                  << ", length=" << std::dec << length 
                  << ", extra=0x" << std::hex << extra << std::endl;
    }
};

// 包类（包含包头和包体）
class Packet {
public:
    PacketHeader header;  // 包头
    std::vector<char> body; // 包体数据

    // 设置包体数据（自动更新包头长度）
    void setBody(const std::vector<char>& data) {
        body = data;
        header.length = static_cast<uint32_t>(body.size());
    }

    // 序列化整个包（包头 + 包体）
    std::vector<char> serialize() const {
        std::vector<char> packet_data;

        // 1. 序列化包头
        std::vector<char> header_data = header.serialize();
        packet_data.insert(packet_data.end(), header_data.begin(), header_data.end());

        // 2. 追加包体数据
        packet_data.insert(packet_data.end(), body.begin(), body.end());

        return packet_data;
    }

    // 反序列化整个包（从字节流解析包头和包体）
    bool deserialize(const std::vector<char>& raw_data) {
        // 1. 检查数据长度是否足够解析包头
        if (raw_data.size() < sizeof(PacketHeader)) {
            std::cerr << "Error: Incomplete header." << std::endl;
            return false;
        }

        // 2. 解析包头
        if (!header.deserialize(raw_data.data())) {
            std::cerr << "Error: Failed to deserialize header." << std::endl;
            return false;
        }

        // 3. 检查魔数是否匹配
        if (header.magic != 0xDEADBEEF) {
            std::cerr << "Error: Invalid magic number." << std::endl;
            return false;
        }

        // 4. 检查包体长度是否匹配
        size_t expected_size = sizeof(PacketHeader) + header.length;
        if (raw_data.size() < expected_size) {
            std::cerr << "Error: Incomplete body." << std::endl;
            return false;
        }

        // 5. 提取包体数据
        const char* body_start = raw_data.data() + sizeof(PacketHeader);
        body.assign(body_start, body_start + header.length);

        return true;
    }

    // 打印整个包信息（调试用）
    void print() const {
        header.print();
        std::cout << "[Body] Size: " << body.size() << " bytes" << std::endl;
        std::cout << "Data: ";
        for (char c : body) std::cout << c;
        std::cout << std::endl;
    }
};

// 示例使用
int main() {
    // 创建包并设置包体
    Packet send_packet;
    std::string message = "Hello, Network Packet!";
    send_packet.setBody(std::vector<char>(message.begin(), message.end()));

    // 序列化为字节流
    std::vector<char> serialized = send_packet.serialize();
    std::cout << "Serialized packet size: " << serialized.size() << " bytes" << std::endl;

    // 模拟网络传输（这里直接传递字节流）
    std::vector<char> received_data = serialized;

    // 接收端反序列化
    Packet recv_packet;
    if (recv_packet.deserialize(received_data)) {
        std::cout << "\nDeserialized successfully:" << std::endl;
        recv_packet.print();
    } else {
        std::cerr << "Failed to deserialize packet." << std::endl;
    }

    return 0;
}