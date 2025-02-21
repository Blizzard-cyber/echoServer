# ifndef EPOOLER_H
# define EPOOLER_H
#include <cstdint>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdexcept>
#include <vector>

class Epoller
{
public:

    /** epoll_create **/
    // 构造函数，创建一个 epoll 实例
    Epoller(int flags = EPOLL_CLOEXEC, int noFile = 1024);

    // 析构函数，关闭 epoll 实例
    ~Epoller();
 
    /** epoll_ctl **/
    // 添加文件描述符到 epoll 实例中
    void addfd(int fd, uint32_t events = EPOLLIN, bool ETorNot = false);

    // 修改 epoll 实例中已存在的文件描述符的事件
    void modfd(int fd, uint32_t events = EPOLLIN, bool ETorNot = false);

    // 从 epoll 实例中删除文件描述符
    void delfd(int fd);

    /** epoll_wait **/
    // 等待事件发生，返回就绪的文件描述符数量
    int wait(int timeout = -1);

    // 获取指定事件的文件描述符
    int getEventOccurfd(int eventIndex) const;

    // 获取指定事件的事件类型
    uint32_t getEvents(int eventIndex) const;
 
    // 检查 epoll 实例是否有效
    bool isValid()
    {
        if (m_epollfd == -1)
            return false;
        return true;
    }

    // 关闭 epoll 实例
    void close()
    {
        if (isValid())
        {
            :: close(m_epollfd);
            m_epollfd = -1;
        }
    }
 
private:
    std::vector<epoll_event> events; // 事件集
    int m_epollfd; // epoll 实例的文件描述符
    int fdNumber; // 文件描述符数量
    int nReady; // 就绪的文件描述符数量
    struct epoll_event event; // 单个事件
};

// 自定义异常类，用于处理 epoll 相关的异常
class EpollException : public std::runtime_error {
    public:
        explicit EpollException(const std::string &msg) : std::runtime_error(msg) {}
    };

#endif // EPOOLER_H