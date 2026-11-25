#include <iostream>
#include <string>

#include "log/logger.hpp"

Logger& Logger::instance() {
  static Logger logger;
  return logger;
}

void Logger::log(LogLevel level, const std::string& message) { std::cout << "[" << level_to_str(level) << "] " << message << std::endl; }

std::string_view Logger::level_to_str(LogLevel level) {
  switch (level) {
  case LogLevel::DEBUG:
    return "DEBUG";
  case LogLevel::INFO:
    return "INFO";
  case LogLevel::ERROR:
    return "ERROR";
  }
}
