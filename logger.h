#ifndef MUDUO_CORE_LOGGER_H
#define MUDUO_CORE_LOGGER_H

#include <algorithm>
#include <list>
#include <memory>
#include <string>

#include "noncopyable.h"
#include "timestamp.h"

#define LOG_FORMAT(level, format, ...)                                    \
  do {                                                                    \
    auto logger = muduo_core::Logger::instance();                         \
    char buf[1024] = {0};                                                 \
    ::snprintf(buf, 1024, format, ##__VA_ARGS__);                         \
    logger->log(level,                                                    \
                muduo_core::LogEvent(__FILE__, __LINE__, __FUNCTION__, buf)); \
  } while (0)

#define LOG_INFO(format, ...) \
  LOG_FORMAT(muduo_core::LogLevel::Level::INFO, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) \
  LOG_FORMAT(muduo_core::LogLevel::Level::ERROR, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) \
  LOG_FORMAT(muduo_core::LogLevel::Level::FATAL, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) \
  LOG_FORMAT(muduo_core::LogLevel::Level::DEBUG, format, ##__VA_ARGS__)

namespace muduo_core {

struct LogLevel {
  enum Level {
    UNKNOWN = 0,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
  };

  static const char* toString(LogLevel::Level level);
  static LogLevel::Level fromString(const std::string& level);
};

class LogEvent {
 public:
  LogEvent(const char* file, const size_t line, const char* func,
           const char* message)
      : timestamp_(Timestamp::now()),
        file_(file),
        line_(line),
        func_(func),
        message_(message) {}

  std::string toString() const;

 private:
  Timestamp timestamp_;
  const char* file_;
  const size_t line_;
  const char* func_;
  const char* message_;
};

class LogAppender : noncopyable {
 public:
  virtual void append(LogLevel::Level, const LogEvent&) = 0;
};

class LogConsoleAppender : public LogAppender {
 public:
  virtual void append(LogLevel::Level, const LogEvent&) override;
};

class Logger : noncopyable {
 public:
  using LogAppenderPtr = std::shared_ptr<LogAppender>;

  static Logger* instance();

  void addAppender(LogAppenderPtr appender) { appenders_.push_back(appender); }

  void removeAppender(LogAppenderPtr appender) {
    for (auto it = appenders_.begin(); it != appenders_.end(); ++it) {
      if (*it == appender) {
        appenders_.erase(it);
        break;
      }
    }
  }

  void enableStdout(bool on) {
    if (on) {
      addAppender(stdout_);
    } else {
      removeAppender(stdout_);
    }
  }

  void setLevel(LogLevel::Level level);

  void log(LogLevel::Level level, const LogEvent& event);
  void debug(const LogEvent& event) { log(LogLevel::Level::DEBUG, event); }
  void info(const LogEvent& event) { log(LogLevel::Level::INFO, event); }
  void warn(const LogEvent& event) { log(LogLevel::Level::WARN, event); }
  void error(const LogEvent& event) { log(LogLevel::Level::ERROR, event); }
  void fatal(const LogEvent& event) { log(LogLevel::Level::FATAL, event); }

 private:
  Logger() : level_(LogLevel::Level::UNKNOWN), stdout_(new LogConsoleAppender) {
    enableStdout(true);
  }

  LogLevel::Level level_;
  LogAppenderPtr stdout_;
  std::list<LogAppenderPtr> appenders_;
};

}  // namespace muduo_core

#endif  // MUDUO_CORE_LOGGER_H
