#ifndef MUDUO_CORE_TCP_SERVER_H
#define MUDUO_CORE_TCP_SERVER_H

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "acceptor.h"
#include "callbacks.h"
#include "event_loop.h"
#include "event_loop_thread.h"
#include "event_loop_thread_pool.h"
#include "inet_address.h"
#include "noncopyable.h"

namespace muduo_core {

class TcpServer : noncopyable {
 public:
  using ThreadInitCallback = std::function<void(EventLoop*)>;

  enum Option {
    kNoReusePort,
    kReusePort,
  };

  TcpServer(EventLoop* loop, const InetAddress& listenAddr,
            const std::string& nameArg, Option option = kNoReusePort);
  ~TcpServer();

  void setThreadInitCallback(const ThreadInitCallback& cb) {
    threadInitCallback_ = cb;
  }
  void setConnectionCallback(const ConnectionCallback& cb) {
    connectionCallback_ = cb;
  }
  void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
  void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
    writeCompleteCallback_ = cb;
  }

  void setThreadNum(int numThreads);

  void start();

 private:
  void newConnection(int sockfd, const InetAddress& peerAddr);
  void removeConnection(const TcpConnectionPtr& conn);
  void removeConnectionInLoop(const TcpConnectionPtr& conn);

  using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;

  EventLoop* loop_;  // main loop

  const std::string ipPort_;
  const std::string name_;

  std::unique_ptr<Acceptor> acceptor_;
  std::shared_ptr<EventLoopThreadPool> threadPool_;

  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  ThreadInitCallback threadInitCallback_;

  std::atomic<int> started_;

  int nextConnId_;
  ConnectionMap connections_;
};

}  // namespace muduo_core

#endif  // MUDUO_CORE_TCP_SERVER_H
