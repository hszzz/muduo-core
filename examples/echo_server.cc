#include <iostream>

#include "../buffer.h"
#include "../tcp_connection.h"
#include "../tcp_server.h"

using namespace muduo_core;

class EchoServer {
 public:
  EchoServer(EventLoop* loop, const InetAddress& addr,
             const std::string nameArg)
      : loop_(loop), server_(loop, addr, nameArg) {
    server_.setConnectionCallback(
        std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
    server_.setMessageCallback(
        std::bind(&EchoServer::onMessage, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));
    server_.setThreadNum(4);
  }

  void start() { server_.start(); }

 private:
  void onConnection(const TcpConnectionPtr& conn) {
    if (conn->connected()) {
      std::cout << "new connection" << std::endl;
    } else {
      std::cout << "disconnection" << std::endl;
    }
  }

  void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time) {
    std::string msg = buffer->retrieveAllAsString();
    conn->send(msg.c_str(), msg.length());
  }

  EventLoop* loop_;
  TcpServer server_;
};

int main() {
  EventLoop loop;
  InetAddress addr(8000);
  EchoServer server(&loop, addr, "echoserver");
  server.start();
  loop.loop();
  return 0;
}
