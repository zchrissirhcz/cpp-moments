#include "gft/log.hpp"

namespace liong {
namespace log {

namespace detail {

void l_default_log_callback__(LogLevel lv, const std::string& msg) {
  switch (lv) {
  case L_LOG_LEVEL_DEBUG:
    printf("[\x1b[90mDEBUG\x1B[0m] %s\n", msg.c_str());
    break;
  case L_LOG_LEVEL_INFO:
    printf("[\x1B[32mINFO\x1B[0m] %s\n", msg.c_str());
    break;
  case L_LOG_LEVEL_WARNING:
    printf("[\x1B[33mWARN\x1B[0m] %s\n", msg.c_str());
    break;
  case L_LOG_LEVEL_ERROR:
    printf("[\x1B[31mERROR\x1B[0m] %s\n", msg.c_str());
    break;
  }
}

LogCallback l_log_callback__ = &l_default_log_callback__;
LogLevel l_filter_lv__ = LogLevel::L_LOG_LEVEL_DEBUG;
std::string l_indent__ = "";

} // namespace detail


void set_log_callback(LogCallback cb) {
  detail::l_log_callback__ = cb;
}
void set_log_filter_level(LogLevel lv) {
  detail::l_filter_lv__ = lv;
}

void push_indent() {
  detail::l_indent__ += "  ";
}
void pop_indent() {
  detail::l_indent__.resize(detail::l_indent__.size() - 2);
}

} // namespace log

} // namespace liong
