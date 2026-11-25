#pragma once

#include <format>
#include <string>

enum class LogLevel { DEBUG, INFO, ERROR };

class Logger {
public:
  static Logger &instance();
  void log(LogLevel level, const std::string &message);

private:
  std::string level_to_str(LogLevel level);
};

namespace logger {
template <typename... Args> void debug(std::format_string<Args...> fmt, Args &&...args) {
  Logger::instance().log(LogLevel::DEBUG, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args> void info(std::format_string<Args...> fmt, Args &&...args) {
  Logger::instance().log(LogLevel::INFO, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args> void error(std::format_string<Args...> fmt, Args &&...args) {
  Logger::instance().log(LogLevel::ERROR, std::format(fmt, std::forward<Args>(args)...));
}
} // namespace logger
