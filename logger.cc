#include "logger.h"

#include <iostream>
#include <chrono>

namespace muduo_core {

Logger* Logger::instance() {
  static Logger logger;
  return &logger;
}

void Logger::setLoggerLevel(LogLevel level) { level_ = level; }

void Logger::log(const std::string& msg) {
  switch (level_) {
    case kINFO:
      std::cout << "INFO";
      break;
    case kERROR:
      std::cout << "ERROR";
      break;
    case kFATAL:
      std::cout << "FATAL";
      break;
    case kDEBUG:
      std::cout << "DEBUG";
      break;
    default:
      break;
  }

  std::cout << " " << "placeholder" << ":" << msg << std::endl;
}

}  // namespace muduo_core
