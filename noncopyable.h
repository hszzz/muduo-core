#ifndef MUDUO_CORE_NOCOPYABLE_H
#define MUDUO_CORE_NOCOPYABLE_H

namespace muduo_core {

class noncopyable {
 public:
  noncopyable(const noncopyable&) = delete;
  noncopyable& operator=(const noncopyable&) = delete;

 protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

}  // namespace muduo_core

#endif  // MUDUO_CORE_NOCOPYABLE_H
