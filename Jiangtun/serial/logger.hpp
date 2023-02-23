#ifndef JIANGTUN_SERIAL_LOGGER_H_
#define JIANGTUN_SERIAL_LOGGER_H_

#include "../logger/logger.hpp"

class SerialLogger : public Logger
{
protected:
    void Print_(const LogLevel level, const char *message) override
    {
        char lv[8];
        switch (level)
        {
        case LogLevel::Debug:
            strcpy(lv, "DEBUG");
            break;
        case LogLevel::Info:
            strcpy(lv, "INFO");
            break;
        case LogLevel::Warning:
            strcpy(lv, "WARNING");
            break;
        case LogLevel::Error:
            strcpy(lv, "ERROR");
            break;
        }

        sprintf(buffer_, "%s\t%s", lv, message);
        Serial.println(buffer_);
    };
private:
    char buffer_[512];
};

#endif // JIANGTUN_SERIAL_LOGGER_H_
