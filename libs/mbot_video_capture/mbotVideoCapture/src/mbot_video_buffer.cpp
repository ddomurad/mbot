#include "inc/mbot_video_buffer.h"
#include <sys/mman.h>

using namespace mbot;
using namespace mbot::video;

VideoBuffer::VideoBuffer():
    buffer(nullptr),
    buffer_length(0)
{}

VideoBuffer::~VideoBuffer()
{
    unmap();
}

result VideoBuffer::map(size_t length, int cam_device, off_t offset)
{
    R_ASSERT(buffer != nullptr);

    buffer_length = length;
    buffer = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, cam_device, offset);

    R_BEGIN_ASSERT_ERRNO(buffer == MAP_FAILED);
        buffer = nullptr;
        buffer_length = 0;
    R_END_ASSERT;

    memset(buffer,0 , buffer_length); //new

    return Success;
}

void VideoBuffer::unmap()
{
    if(buffer != nullptr)
    {
        munmap(buffer, buffer_length);
        buffer = nullptr;
        buffer_length = 0;
    }
}
