#ifndef CORE_UTILS_LOGGER_H
#define CORE_UTILS_LOGGER_H

#include <string>
#include <iostream>

namespace core {
namespace utils {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static Logger& getInstance();

    void log(LogLevel level, const std::string& message);
    void setLogLevel(LogLevel level);

private:
    Logger() = default;
    LogLevel currentLevel = LogLevel::INFO;

    std::string levelToString(LogLevel level);
};

} // namespace utils
} // namespace core

#endif // CORE_UTILS_LOGGER_H
