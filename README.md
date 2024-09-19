本项目简介：抱着学习的思想，基于陈硕大神的muduo库进行重构，实现了大部分功能的基础上，新增加了解析http请求的功能，实现了一个mimi的http服务器，并利用定时器实现了心跳检测的功能 后期再慢慢添加讲解说明  由浅入深剖析muduo网络库。

2024.9.19更新 ： 应用层利用内部提供的定时器实现心跳检测功能。




****************************************************************************

整体架构图：
![图片](https://github.com/user-attachments/assets/3776241a-a070-4571-b4bc-aa3c9950cd0a)

## 好的网络服务器如何设计

在这个多核时代，服务端网络编程如何选择线程模型呢？ 赞同libev作者的观点：one loop per thread is usually a good model，这样多线程服务端编程的问题就转换为如何设计一个高效且易于使用的event loop，然后每个线程run一个event loop就行了（当然线程间的同步、互斥少不了，还有其它的耗时事件需要起另外的线程来做）
强大的nginx服务器采用了epoll+fork模型作为网络模块的架构设计，实现了简单好用的负载算法，使各个fork网络进程不会忙的越忙、闲的越闲，并且通过引入一把乐观锁解决了该模型导致的 服务器惊群现象，功能十分强大。

event loop 是 non-blocking 网络编程的核心，在现实生活中，non-blocking 几乎总是和 IO-multiplexing 一起使用，原因有两点：

- 没有人真的会用轮询 (busy-pooling) 来检查某个 non-blocking IO 操作是否完成，这样太浪费 CPU资源了。
- IO-multiplex 一般不能和 blocking IO 用在一起，因为 blocking IO 中 read()/write()/accept()/connect() 都有可能阻塞当前线程，这样线程就没办法处理其他 socket 上的 IO 事件了。

所以，当我们提到 non-blocking 的时候，实际上指的是 non-blocking + IO-multiplexing，单用其中任何一个都没有办法很好的实现功能

## Nginx为何是epoll+fork

强大的nginx服务器采用了epoll+fork模型作为网络模块的架构设计，实现了简单好用的负载算法，使各个fork网络进程不会忙的越忙、闲的越闲，并且通过引入一把乐观锁解决了该模型导致的 服务器惊群现象，功能十分强大
