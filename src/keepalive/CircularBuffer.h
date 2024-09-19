#pragma once

#include <vector>
// #include <cstddef>

template <typename T>
class CircularBuffer 
{
public:
    CircularBuffer(size_t size) : buffer(size), maxSize(size), head(0), tail(0), full(false) {}


    void enqueue(T item) {
        buffer[tail] = item;
        if (full) {
            head = (head + 1) % maxSize; // 覆盖最旧的元素
        }
        tail = (tail + 1) % maxSize;
        full = tail == head;
    }


    //查看队尾元素，不删除
    T& back() {
        if (isEmpty()) {
            LOG_ERROR("从空缓冲区查看");
        }
        return buffer[tail];
    }

    bool isFull() const {
        return full;
    }

    bool isEmpty() const {
        return (!full && (head == tail));
    }

private:
    std::vector<T> buffer;
    size_t maxSize;
    size_t head;
    size_t tail;
    bool full;
};

