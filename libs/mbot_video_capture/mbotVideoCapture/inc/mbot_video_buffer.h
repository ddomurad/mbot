#ifndef ROVER_VIDEO_BUFFER_H
#define ROVER_VIDEO_BUFFER_H

#include <mbot/core/mbot_defs.h>

namespace mbot {
namespace  video {

class VideoBuffer
{
public:
    VideoBuffer();
    virtual ~VideoBuffer();

    void * buffer;
    size_t buffer_length;

    result map(size_t length, int cam_device, off_t offset);
    void unmap();
};

}
}

#endif // ROVER_VIDEO_BUFFER_H
