#ifndef MIBOT_LOGGER_H
#define MIBOT_LOGGER_H

#include "mbot_defs.h"
#include "mbot_logger_defs.h"

namespace mbot {

class LoggerConfig
{
public:
    LoggerConfig();
    LoggerConfig(std::string applciationName);
    LoggerConfig(std::string applciationName, LOG_LEVEL level, bool stdOut);

    std::string applciationName;
    LOG_LEVEL logLevel;
    bool enableStdOut;
};

class Logger
{
public:
    static void init(LoggerConfig config);
    static void write(LOG_LEVEL level, const char *file_name, const char *func_name, int line, const char *format, ...);
};

}


#endif // MIBOT_LOGGER_H
