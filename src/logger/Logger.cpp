#include "logger/Logger.hpp"
#include <iostream>
#include <chrono>
#include <format>
#include <string_view>

Logger::Logger(const std::string& filename) {
    if (!filename.empty()) {
        m_file.open(filename, std::ios::app);
    }
}

Logger::~Logger() {
    if (m_file.is_open()) m_file.close();
}

std::string Logger::FormatMessage(const LogLevel level, const std::string& message) {
    // Get current system time
    const auto now = std::chrono::system_clock::now();
    
    // Map LogLevel to a readable string
    std::string_view levelStr;
    switch (level) {
        case LogLevel::Trace:    levelStr = "TRACE"; break;
        case LogLevel::Debug:    levelStr = "DEBUG"; break;
        case LogLevel::Info:     levelStr = "INFO"; break;
        case LogLevel::Warning:  levelStr = "WARN"; break;
        case LogLevel::Error:    levelStr = "ERROR"; break;
        case LogLevel::Critical: levelStr = "CRITICAL"; break;
        default:                 levelStr = "LOG"; break;
    }

    // Modern C++20 formatting: [YYYY-MM-DD HH:MM:SS.ms] [LEVEL] Message
    return std::format("[{:%F %T}] [{}] {}", now, levelStr, message);
}

void Logger::Log(const LogLevel level, const std::string& message) {
    const std::string formatted = FormatMessage(level, message);

    // Thread-safety: Lock the mutex before writing to shared resources (file/console)
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_file.is_open()) {
        m_file << formatted << std::endl;
    } else {
        std::cout << formatted << std::endl;
    }
}
