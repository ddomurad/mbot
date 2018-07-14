#ifndef MIBOT_LOGGER_DEFS
#define MIBOT_LOGGER_DEFS

namespace mbot
{

enum class LOG_LEVEL {
    TRACE   = 0,
    DEBUG   = 1,
    INFO    = 2,
    WARNING = 3,
    ERROR   = 4,
    FATAL   = 5,
};

} // logger

#define LOG_TRACE(...) mbot::Logger::write(mbot::LOG_LEVEL::TRACE, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) mbot::Logger::write(mbot::LOG_LEVEL::DEBUG, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define LOG_INFO(...) mbot::Logger::write(mbot::LOG_LEVEL::INFO, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) mbot::Logger::write(mbot::LOG_LEVEL::WARNING, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) mbot::Logger::write(mbot::LOG_LEVEL::ERROR, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) mbot::Logger::write(mbot::LOG_LEVEL::FATAL, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#endif // MIBOT_LOGGER_DEFS
