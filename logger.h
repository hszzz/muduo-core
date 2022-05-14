#ifndef MUDUO_CORE_LOGGER_H
#define MUDUO_CORE_LOGGER_H

#include <string>
#include <stdio.h>

#include "noncopyable.h"

#define LOG_FORMAT(level, format, ...)            \
  do {                                            \
    auto logger = muduo_core::Logger::instance(); \
    logger->setLoggerLevel(level);                \
    char buf[1024] = {0};                         \
    ::snprintf(buf, 1024, format, ##__VA_ARGS__); \
    logger->log(buf);                             \
  } while (0)

#define LOG_INFO(format, ...) LOG_FORMAT(muduo_core::kINFO, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) LOG_FORMAT(muduo_core::kERROR, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) LOG_FORMAT(muduo_core::kFATAL, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) LOG_FORMAT(muduo_core::kDEBUG, format, ##__VA_ARGS__)

namespace muduo_core {
enum LogLevel {
  kINFO = 0,
  kERROR,
  kFATAL,
  kDEBUG,
};

class Logger : noncopyable {
 public:
  static Logger* instance();
  void setLoggerLevel(LogLevel level);
  void log(const std::string& msg);

 private:
  LogLevel level_;
  Logger() {}
};

}  // namespace muduo_core

#endif  // MUDUO_CORE_LOGGER_H
