#ifndef MUDUO_CORE_LOGGER_H
#define MUDUO_CORE_LOGGER_H

#include <algorithm>
#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "current_thread.h"
#include "noncopyable.h"
#include "thread.h"
#include "timestamp.h"

#define LOG_FORMAT(level, format, ...)                              \
  do {                                                              \
    auto logger = muduo_core::Logger::instance();                   \
    char buf[1024] = {0};                                           \
    ::snprintf(buf, 1024, format, ##__VA_ARGS__);                   \
    logger->log(level, std::make_shared<muduo_core::LogEvent>(      \
                           __FILE__, __LINE__, __FUNCTION__, muduo_core::CurrentThread::tid(), buf)); \
  } while (0)

#define LOG_DEBUG(format, ...) \
  LOG_FORMAT(muduo_core::LogLevel::Level::DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) \
  LOG_FORMAT(muduo_core::LogLevel::Level::INFO, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) \
  LOG_FORMAT(muduo_core::LogLevel::Level::WARN, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) \
  LOG_FORMAT(muduo_core::LogLevel::Level::ERROR, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) \
  LOG_FORMAT(muduo_core::LogLevel::Level::FATAL, format, ##__VA_ARGS__)

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
  using LogEventPtr = std::shared_ptr<LogEvent>;

  LogEvent(const char* file, const size_t line, const char* func, int tid,
           const char* message)
      : timestamp_(Timestamp::now()),
        file_(file),
        line_(line),
        func_(func),
        tid_(tid),
        message_(message) {}

  std::string toString() const;

 private:
  Timestamp timestamp_;
  const char* file_;
  const size_t line_;
  const char* func_;
  int tid_;
  std::string message_;
};

class LogAppender : noncopyable {
 public:
  virtual void append(LogLevel::Level, LogEvent::LogEventPtr) = 0;
};

class LogConsoleAppender : public LogAppender {
 public:
  virtual void append(LogLevel::Level, LogEvent::LogEventPtr) override;
};

class Logger : noncopyable {
 public:
  using LogAppenderPtr = std::shared_ptr<LogAppender>;

  static Logger* instance();

  void addAppender(LogAppenderPtr appender) {
    std::unique_lock<std::mutex> lock(mutex_);
    appenders_.push_back(appender);
  }

  void removeAppender(LogAppenderPtr appender) {
    std::unique_lock<std::mutex> lock(mutex_);
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

  void log(LogLevel::Level level, LogEvent::LogEventPtr event);

  void debug(LogEvent::LogEventPtr event) {
    log(LogLevel::Level::DEBUG, event);
  }
  void info(LogEvent::LogEventPtr event) { log(LogLevel::Level::INFO, event); }
  void warn(LogEvent::LogEventPtr event) { log(LogLevel::Level::WARN, event); }
  void error(LogEvent::LogEventPtr event) {
    log(LogLevel::Level::ERROR, event);
  }
  void fatal(LogEvent::LogEventPtr event) {
    log(LogLevel::Level::FATAL, event);
  }

  ~Logger() {
    stop_ = true;
    thread_.join();
  }

 private:
  using LogEventPair = std::pair<LogLevel::Level, LogEvent::LogEventPtr>;

  Logger(LogLevel::Level level)
      : stop_(false),
        level_(level),
        stdout_(new LogConsoleAppender),
        thread_(std::bind(&Logger::asyncLog, this), "LOGGER") {
    enableStdout(true);
    thread_.start();
  }

  bool stop_;

  LogLevel::Level level_;
  LogAppenderPtr stdout_;

  void asyncLog();
  Thread thread_;

  std::mutex mutex_;
  std::list<LogAppenderPtr> appenders_;

  std::condition_variable cond_;
  std::queue<LogEventPair> queue_;
};

}  // namespace muduo_core

#endif  // MUDUO_CORE_LOGGER_H
