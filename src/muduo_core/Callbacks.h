#pragma once

#include <memory>
#include <functional>

class Buffer;
class TcpConnection;
class Timestamp;
class myEntry;

using WeakTcpConnectionPtr = std::weak_ptr<TcpConnection>;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void (const TcpConnectionPtr&)>;
using CloseCallback = std::function<void (const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void (const TcpConnectionPtr&)>;
using MessageCallback = std::function<void (const TcpConnectionPtr&,
                                        Buffer*,
                                        Timestamp)>;
using HighWaterMarkCallback = std::function<void (const TcpConnectionPtr&, size_t)>;

using EntryPtr = std::shared_ptr<myEntry>;
using WeakEntryPtr = std::weak_ptr<myEntry>;