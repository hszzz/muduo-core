#ifndef MUDUO_CORE_CALLBACKS_H
#define MUDUO_CORE_CALLBACKS_H

#include <functional>
#include <memory>

namespace muduo_core {

class Buffer;
class TcpConnection;
class Timestamp;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback =
    std::function<void(const TcpConnectionPtr&, Buffer*, Timestamp)>;
using HighWaterMarkCallback =
    std::function<void(const TcpConnectionPtr&, size_t)>;

}  // namespace muduo_core

#endif  //  MUDUO_CORE_CALLBACKS_H
