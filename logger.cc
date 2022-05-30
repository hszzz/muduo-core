#include "logger.h"

#include <chrono>
#include <iostream>

#include "current_thread.h"

namespace muduo_core {

#define LOG_LEVEL LogLevel::Level::ERROR

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
  ::snprintf(buf, sizeof(buf), " %s %d %s:%ld:%s -> %s",
             timestamp_.toString().c_str(), tid_, file_, line_, func_,
             message_.c_str());
  return buf;
}

void LogConsoleAppender::append(LogLevel::Level level,
                                LogEvent::LogEventPtr event) {
  std::cout << LogLevel::toString(level) << event->toString() << std::endl;
}

Logger* Logger::instance() {
  static Logger logger(LOG_LEVEL);
  return &logger;
}

void Logger::setLevel(LogLevel::Level level) { level_ = level; }

void Logger::log(LogLevel::Level level, LogEvent::LogEventPtr event) {
  if (level > level_) {
    std::unique_lock<std::mutex> lock(mutex_);
    queue_.push({level, event});
    cond_.notify_all();
  }
}

void Logger::asyncLog() {
  while (1) {
    LogLevel::Level level;
    LogEvent::LogEventPtr event = nullptr;

    {
      std::unique_lock<std::mutex> lock(mutex_);
      while (queue_.empty()) cond_.wait(lock);
      level = queue_.front().first;
      event = queue_.front().second;
      queue_.pop();
    }

    for (const auto& appender : appenders_) {
      appender->append(level, event);
    }

    if (level == LogLevel::Level::FATAL) ::abort();

    if (stop_ == true && queue_.empty()) {
      break;
    }
  }
}

}  // namespace muduo_core
