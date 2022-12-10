// # Assertion
// @PENGUINLIONG
#pragma once
#include "gft/util.hpp"

namespace liong {

class AssertionFailedException : public std::exception {
  const char* file;
  uint32_t line;
  std::string msg;

public:
  inline AssertionFailedException(
    const char* file,
    uint32_t line,
    const std::string& msg
  ) : file(file), line(line), msg(msg) {}

  const char* what() const noexcept override {
    return msg.c_str();
  }
};

#ifdef NDEBUG
// Release configs.
#define L_ASSERT(pred, ...)
#define L_PANIC(pred, ...)
#else
// Debug configs.
#define L_ASSERT(pred, ...) \
  if (!(pred)) { \
    throw liong::AssertionFailedException(__FILE__, __LINE__, liong::util::format(__VA_ARGS__)); \
  }
#define L_PANIC(...) L_ASSERT(false, __VA_ARGS__)
#endif

template<typename ... TArgs>
[[noreturn]] inline void panic(const TArgs& ... args) {
  L_ASSERT(false, args ...);
}
template<typename ... TArgs>
[[noreturn]] inline void unreachable(const TArgs& ... args) {
  L_ASSERT(false, "reached unreachable code: ", args ...);
}
[[noreturn]] inline void unimplemented() {
  L_ASSERT(false, "reached unimplemented code");
}


} // namespace liong
