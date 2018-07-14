#ifndef ROVER_VIDEO_CONFIG_H
#define ROVER_VIDEO_CONFIG_H

#include <mbot/core/mbot_defs.h>

namespace mbot {
namespace video {

enum class video_format { YUV420M, /*YUV422M,*/ H264, MJPEG };

typedef result (*frame_handler_callback)(void * data, uint32_t len);

class VideoConfig
{
public:
    VideoConfig()
    {}

    VideoConfig(
            std::string device_name,uint16_t max_buffers,
            uint16_t frame_width, uint16_t frame_height, video_format format,
            frame_handler_callback frame_handler):
        device_name(device_name),
        max_buffers(max_buffers),
        frame_width(frame_width),
        frame_height(frame_height),
        format(format),
        frame_handler(frame_handler)
    {}

    std::string device_name;
    uint16_t max_buffers;
    uint16_t frame_width;
    uint16_t frame_height;
    video_format format;
    frame_handler_callback frame_handler;
};

}
}
#endif //ROVER_VIDEO_CONFIG_H
