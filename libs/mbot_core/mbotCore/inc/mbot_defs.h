#ifndef ROVER_DEFS_H
#define ROVER_DEFS_H

#include "string"

#include "inttypes.h"
#include "errno.h"
#include "string.h"

#include "mbot_logger.h"

namespace mbot {

    typedef uint8_t byte;
    typedef int8_t sbyte;
    typedef uint8_t result;

    const result Yes = 0x01;
    const result No = 0x00;

    const result Error = 0x00;
    const result Success = 0x01;
}


#define ASSERT(...) if(__VA_ARGS__) {LOG_FATAL("%s", #__VA_ARGS__); return;}
#define BEGIN_ASSERT(...) if(__VA_ARGS__) {LOG_FATAL("%s", #__VA_ARGS__);
#define END_ASSERT return;}

#define R_ASSERT(...) if(__VA_ARGS__) {LOG_FATAL("%s", #__VA_ARGS__); return mbot::Error;}
#define R_ASSERT_ERRNO(...) if(__VA_ARGS__) {LOG_FATAL("%s -> %s", #__VA_ARGS__, strerror(errno)); return mbot::Error;}

#define R_BEGIN_ASSERT(...) if(__VA_ARGS__) {LOG_FATAL("%s", #__VA_ARGS__);
#define R_BEGIN_ASSERT_ERRNO(...) if(__VA_ARGS__) {LOG_FATAL("%s -> %s", #__VA_ARGS__, strerror(errno));

#define R_END_ASSERT return mbot::Error;}
#define RX_END_ASSERT(ERR) return ERR;}

#endif //ROVER_DEFS_H
