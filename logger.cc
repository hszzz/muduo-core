#include "logger.h"

#include <chrono>
#include <iostream>

namespace muduo_core {

const char* LogLevel::toString(LogLevel::Level level) {
  switch (level) {
#define XX(name)       \
  case LogLevel::name: \
    return #name;      \
    break;

    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX
    default:
      return "UNKNOW";
  }
  return "UNKNOW";
}

LogLevel::Level LogLevel::fromString(const std::string& str) {
#define XX(level, v)        \
  if (str == #v) {          \
    return LogLevel::level; \
  }

  XX(DEBUG, debug);
  XX(INFO, info);
  XX(WARN, warn);
  XX(ERROR, error);
  XX(FATAL, fatal);

  XX(DEBUG, DEBUG);
  XX(INFO, INFO);
  XX(WARN, WARN);
  XX(ERROR, ERROR);
  XX(FATAL, FATAL);

  return LogLevel::UNKNOWN;
#undef XX
}

std::string LogEvent::toString() const {
  char buf[1024] = {0};
  ::snprintf(buf, sizeof(buf), " %s %s:%ld:%s -> %s",
             timestamp_.toString().c_str(), file_, line_, func_, message_);
  return buf;
}

void LogConsoleAppender::append(LogLevel::Level level, const LogEvent& event) {
  std::cout << LogLevel::toString(level) << event.toString() << std::endl;
}

Logger* Logger::instance() {
  static Logger logger;
  return &logger;
}

void Logger::setLevel(LogLevel::Level level) { level_ = level; }

void Logger::log(LogLevel::Level level, const LogEvent& event) {
  if (level > level_) {
    for (const auto& appender : appenders_) {
      appender->append(level, event);
    }
  }
  if (level == LogLevel::Level::FATAL) ::abort();
}

}  // namespace muduo_core
