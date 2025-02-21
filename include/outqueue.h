#ifndef OUTQUEUE_H
#define OUTQUEUE_H

#include <cstddef>
#include <queue>
#include <stdexcept>

class OutQueue {
private:
    enum State {
        COMPLETE_RECEIVED, // 报文全部到达(含包头)
        PARTIAL_RECEIVED, // 报文部分到达(含包头)
        REMAINING_RECEIVED, // 报文剩余部分到达(不含包头)
    };

    struct task {
        int packetID; // 包ID
        char* buffer; // 缓冲区地址
        char* offset; // 要发送的数据地址
        size_t outSize; // 发送数据的大小
        State packState; // 状态
    };

    std::queue<task> taskQueue; // 任务队列

public:
    // 构造函数
    OutQueue() {}

    // 添加任务到发送队列
    void addTask(int packetID, char* buffer, size_t size, State state) {
        task newTask = {packetID, buffer, buffer, size, state};
        taskQueue.push(newTask);
    }

    // 获取当前发送数据的指针
    char* getOutData() {
        if (!taskQueue.empty()) {
            return taskQueue.front().offset;
        }
        return nullptr;
    }

    // 获取当前发送数据的大小
    size_t getOutSize() {
        if (!taskQueue.empty()) {
            return taskQueue.front().outSize;
        }
        return 0;
    }

    // 更新已发送的数据大小
    void updateSentSize(size_t sentSize) {
        if (!taskQueue.empty()) {
            taskQueue.front().offset += sentSize;
            taskQueue.front().outSize -= sentSize;
            if (taskQueue.front().outSize == 0) {
                taskQueue.pop(); // 移除已发送完的数据包
            }
        }
    }

    // 检查发送队列是否为空
    bool isEmpty() const {
        return taskQueue.empty();
    }

    // 获取当前任务的状态
    State getCurrentState() const {
        if (!taskQueue.empty()) {
            return taskQueue.front().packState;
        }
        throw std::runtime_error("Task queue is empty");
    }
};

#endif // OUTQUEUE_H