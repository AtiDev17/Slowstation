#pragma once
#include "ILogger.hpp"
#include <fstream>
#include <mutex>


class Logger : public ILogger {
public:
    explicit Logger(const std::string& filename = "");
    ~Logger() override;

    void Log(LogLevel level, const std::string& message) override;

private:
    std::ofstream m_file;
    std::mutex m_mutex;
    static std::string FormatMessage(LogLevel level, const std::string& message);
};