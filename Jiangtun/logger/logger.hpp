#ifndef JIANGTUN_LOGGER_H_
#define JIANGTUN_LOGGER_H_

enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error,
};

class Logger
{
public:
    void SetLevel(const LogLevel level)
    {
        level_ = level;
    }
    void Debug(const char *message)
    {
        Print__(LogLevel::Debug, message);
    }
    void Info(const char *message)
    {
        Print__(LogLevel::Info, message);
    }
    void Warning(const char *message)
    {
        Print__(LogLevel::Warning, message);
    }
    void Error(const char *message)
    {
        Print__(LogLevel::Error, message);
    }

protected:
    virtual void Print_(const LogLevel level, const char *message) {}

private:
    LogLevel level_ = LogLevel::Debug;
    void Print__(const LogLevel level, const char *message)
    {
        if (level < level_)
        {
            return;
        }
        Print_(level, message);
    }
};

#endif // JIANGTUN_LOGGER_H_
