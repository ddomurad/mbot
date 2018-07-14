#include <cstdio>
#include <cstdarg>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "inc/mbot_logger.h"

using namespace mbot;

LoggerConfig::LoggerConfig() :
    applciationName("unknown"),
    logLevel(LOG_LEVEL::WARNING),
    enableStdOut(true)
{}

LoggerConfig::LoggerConfig(std::string applciationName) :
    applciationName(applciationName),
    logLevel(LOG_LEVEL::WARNING),
    enableStdOut(true)
{}

LoggerConfig::LoggerConfig(std::string applciationName, LOG_LEVEL level, bool stdOut) :
    applciationName(applciationName),
    logLevel(level),
    enableStdOut(stdOut)
{}


LoggerConfig _config;


struct tcp_connection_desc
{
    int sockfd;
    struct sockaddr_in serv_addr;
} tcp_connection_desc;


const char * _get_log_level_str(LOG_LEVEL level);
const char * _trim_path( const char *file_name);
void _connect_to_logger_host();
void _disconnect_from_logger_host();

void Logger::init(LoggerConfig config)
{
    _config = config;
}

void Logger::write(LOG_LEVEL level, const char *file_name, const char *func_name, int line, const char *format, ...)
{
    if(level < _config.logLevel)
        return;

    const char *file_name_ptr = _trim_path(file_name);

    va_list arg;

    if(_config.enableStdOut)
    {
        printf("[%s] @ %s (%s:%s[%d]): ", _get_log_level_str(level), _config.applciationName.c_str(), file_name_ptr, func_name, line);
        va_start (arg, format);
        vfprintf (stdout, format, arg);
        printf("\n");
        va_end (arg);
    }
}

const char * _get_log_level_str(LOG_LEVEL level)
{
    switch(level)
    {
        case LOG_LEVEL::TRACE: return "TRC";
        case LOG_LEVEL::DEBUG: return "DBG";
        case LOG_LEVEL::INFO: return "INF";
        case LOG_LEVEL::WARNING: return "WRN";
        case LOG_LEVEL::ERROR: return "ERR";
        case LOG_LEVEL::FATAL: return "FAT";
        default: return "UNKNOWN";
    }
}

const char* _trim_path(const char *file_name)
{
    size_t str_offset = strlen(file_name) - 1;
    if(str_offset < 5)
        return file_name;

    str_offset -= 5;

    while(str_offset > 0 && (file_name[str_offset-1] != '.' && file_name[str_offset-1] != '/' && file_name[str_offset-1] != '\\'))
        str_offset--;

    if(str_offset == 0)
        return file_name;

    return file_name + (str_offset);
}

void _tcp_error(const char * format, ...)
{
    va_list arg;

    va_start (arg, format);
    vfprintf (stdout, format, arg);
    printf("\n");
    va_end (arg);

    exit(1);
}

