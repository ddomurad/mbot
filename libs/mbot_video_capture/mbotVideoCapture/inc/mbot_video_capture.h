#ifndef ROVER_VIDEO_CAPTURE_H
#define ROVER_VIDEO_CAPTURE_H

#include <mbot/core/mbot_defs.h>
#include "mbot_video_config.h"
#include "mbot_video_buffer.h"

#include <pthread.h>
#include <vector>

namespace mbot {
namespace video {

class VideoCapture
{
private:
    //state
    bool _device_opened;
    bool _should_streaming;
    bool _is_streaming;
    int _cam_device;
    std::vector<VideoBuffer*> _video_buffers;

    //configuration
    VideoConfig _config;

    //thread
    pthread_t _capture_thread;
public:
    VideoCapture();
    ~VideoCapture();

    result open(VideoConfig config);
    result enable();
    void disable();
    void close();

private:
    result _enable_stream();
    result _disable_stream();
    result _next_frame();

    result _queueBuffer(uint16_t index);
    void _thread_entry();
    static void *_static_thread_entry(void *self);
};

}
}

#endif //ROVER_VIDEO_CAPTURE_H
