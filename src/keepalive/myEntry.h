#pragma once

#include "TcpConnection.h"
#include "CircularBuffer.h"
#include <unordered_set>

struct myEntry
{
    explicit myEntry(const WeakTcpConnectionPtr& weakConn) : weakConn_(weakConn) {}
    ~myEntry()
    {
        TcpConnectionPtr conn = weakConn_.lock();
        if (conn) conn->shutdown();
    }

	WeakTcpConnectionPtr weakConn_;
};

using Bucket = std::unordered_set<EntryPtr>;
using WeakConnectionList = CircularBuffer<Bucket>;  // 时间轮


