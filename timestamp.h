#ifndef MUDUO_CORE_TIMESTAMP_H
#define MUDUO_CORE_TIMESTAMP_H

#include <ctime>
#include <string>

namespace muduo_core {

class Timestamp {
 public:
  Timestamp() : timestamp_(0) {}
  explicit Timestamp(int64_t time) : timestamp_(time) {}
  std::string toString() const;

  static Timestamp now() { return Timestamp(time(nullptr)); }

 private:
  int64_t timestamp_;
};

}  // namespace muduo_core

#endif  // MUDUO_CORE_TIMESTAMP_H
