// Logging infrastructure.
// @PENGUINLIONG
#pragma once
#include <string>
#include "gft/util.hpp"

#ifndef L_MIN_LOG_LEVEL
#define L_MIN_LOG_LEVEL 0
#endif // L_MIN_LOG_LEVEL

namespace liong {
namespace log {

enum LogLevel {
  L_LOG_LEVEL_DEBUG = 0,
  L_LOG_LEVEL_INFO = 1,
  L_LOG_LEVEL_WARNING = 2,
  L_LOG_LEVEL_ERROR = 3,
};

typedef void (*LogCallback)(LogLevel lv, const std::string& msg);

namespace detail {

extern LogCallback l_log_callback__;
extern LogLevel l_filter_lv__;
extern std::string l_indent__;

} // namespace detail

void set_log_callback(LogCallback cb);
void set_log_filter_level(LogLevel lv);
template<typename ... TArgs>
void log(LogLevel lv, const TArgs& ... msg) {
  if (detail::l_log_callback__ != nullptr && lv >= detail::l_filter_lv__) {
    detail::l_log_callback__(lv, util::format(detail::l_indent__, msg...));
  }
}

void push_indent();
void pop_indent();

template<typename ... TArgs>
inline void debug(const TArgs& ... msg) {
#ifdef L_MIN_LOG_LEVEL
  if constexpr ((int)LogLevel::L_LOG_LEVEL_DEBUG >= L_MIN_LOG_LEVEL)
#endif // L_MIN_LOG_LEVEL
  log(LogLevel::L_LOG_LEVEL_DEBUG, msg...);
}
template<typename ... TArgs>
inline void info(const TArgs& ... msg) {
#ifdef L_MIN_LOG_LEVEL
  if constexpr ((int)LogLevel::L_LOG_LEVEL_INFO >= L_MIN_LOG_LEVEL)
#endif // L_MIN_LOG_LEVEL
  log(LogLevel::L_LOG_LEVEL_INFO, msg...);
}
template<typename ... TArgs>
inline void warn(const TArgs& ... msg) {
#ifdef L_MIN_LOG_LEVEL
  if constexpr ((int)LogLevel::L_LOG_LEVEL_WARNING >= L_MIN_LOG_LEVEL)
#endif // L_MIN_LOG_LEVEL
  log(LogLevel::L_LOG_LEVEL_WARNING, msg...);
}
template<typename ... TArgs>
inline void error(const TArgs& ... msg) {
#ifdef L_MIN_LOG_LEVEL
  if constexpr ((int)LogLevel::L_LOG_LEVEL_ERROR >= L_MIN_LOG_LEVEL)
#endif // L_MIN_LOG_LEVEL
  log(LogLevel::L_LOG_LEVEL_ERROR, msg...);
}

} // namespace log
} // namespace liong

#if !defined(L_MIN_LOG_LEVEL) || L_MIN_LOG_LEVEL <= 0
#define L_DEBUG(...) ::liong::log::debug(__VA_ARGS__)
#else
#define L_DEBUG(...)
#endif // defined(L_MIN_LOG_LEVEL) && L_MIN_LOG_LEVEL >= 0

#if !defined(L_MIN_LOG_LEVEL) || L_MIN_LOG_LEVEL <= 1
#define L_INFO(...) ::liong::log::info(__VA_ARGS__)
#else
#define L_INFO(...)
#endif // defined(L_MIN_LOG_LEVEL) && L_MIN_LOG_LEVEL >= 1

#if !defined(L_MIN_LOG_LEVEL) || L_MIN_LOG_LEVEL <= 2
#define L_WARN(...) ::liong::log::warn(__VA_ARGS__)
#else
#define L_WARN(...)
#endif // defined(L_MIN_LOG_LEVEL) && L_MIN_LOG_LEVEL >= 2

#if !defined(L_MIN_LOG_LEVEL) || L_MIN_LOG_LEVEL <= 3
#define L_ERROR(...) ::liong::log::error(__VA_ARGS__)
#else
#define L_ERROR(...)
#endif // defined(L_MIN_LOG_LEVEL) && L_MIN_LOG_LEVEL >= 3
