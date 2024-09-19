#include "TcpServer.h"
#include "Logger.h"
#include "myEntry.h"

#include <string>
#include <functional>
#include <thread>


class Echoserver
{
public:
    Echoserver(EventLoop *loop,
            const InetAddress &addr, 
            int idleSeconds,
            const std::string &name)
        : server_(loop, addr, name)
        , loop_(loop)
        ,connectionBuckets_(idleSeconds)
    {
        // 注册回调函数
        server_.setConnectionCallback(
            std::bind(&Echoserver::onConnection, this, std::placeholders::_1)
        );

        server_.setMessageCallback(
            std::bind(&Echoserver::onMessage, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );
        
        // 设置合适的loop线程数量 loopthread
        server_.setThreadNum(std::thread::hardware_concurrency()-4);
    }
    void start()
    {
        server_.start();
        loop_->loop(); // 启动mainLoop的底层Poller
        loop_->runEvery(1.0, std::bind(&Echoserver::onTimer, this));
    }
private:
// 有新连接到来时，需要把连接添加到队尾的桶中
    void onConnection(const TcpConnectionPtr& conn)
    {
        if (conn->connected())
        {
            // 创建一个Entry，用来记录连接，即conn，Entry的析构函数会关闭连接
            EntryPtr entry(new myEntry(conn));
            // 在时间轮的队尾的桶中插入Entry，可简单理解为插入连接
            connectionBuckets_.back().insert(entry);
            WeakEntryPtr weakEntry(entry);
            conn->setContext(weakEntry);
        }
    }
    // 有新消息来的时候，把连接添加到队尾的桶中
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
    {
        std::string msg = buf->retrieveAllAsString();
        std::cout << msg << std::endl;
        conn->send(msg);
        WeakEntryPtr weakEntry(conn->getContext());
        EntryPtr entry(weakEntry.lock());
        if (entry)
        {
            // 在时间轮的队尾的桶中插入Entry，可简单理解为插入连接
            connectionBuckets_.back().insert(entry);
        }
    }
        // 超时回调函数
    void onTimer()
    {
        // 在时间轮队尾添加一个新的空桶，这样队首的桶自然被弹出，桶中的Entry引用计数为0
        // 就会被析构，Entry析构又会关闭连接
        connectionBuckets_.enqueue(Bucket());
    }

    

    EventLoop *loop_;
    TcpServer server_;
    WeakConnectionList connectionBuckets_; // 时间轮
};

int main()
{
    EventLoop loop;
    InetAddress addr(12345);
    Echoserver server(&loop, addr, 8,"Echotest"); // Acceptor non-blocking listenfd  create bind 
    server.start(); // listen  loopthread  listenfd => acceptChannel => mainLoop =>

    return 0;
}