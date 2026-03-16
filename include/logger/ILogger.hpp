#pragma once
#include <string>

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void Log(LogLevel level, const std::string& message) = 0;

    // Optional convenience methods
    virtual void Trace(const std::string& message) { Log(LogLevel::Trace, message); }
    virtual void Debug(const std::string& message) { Log(LogLevel::Debug, message); }
    virtual void Info(const std::string& message) { Log(LogLevel::Info, message); }
    virtual void Warning(const std::string& message) { Log(LogLevel::Warning, message); }
    virtual void Error(const std::string& message) { Log(LogLevel::Error, message); }
    virtual void Critical(const std::string& message) { Log(LogLevel::Critical, message); }
};