#include "inc/mbot_video_capture.h"

#include <linux/videodev2.h>

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

using namespace mbot;
using namespace mbot::video;

__u32 _get_vformat(video_format format);

VideoCapture::VideoCapture() :
    _device_opened(No),
    _should_streaming(No),
    _is_streaming(No),
    _cam_device(-1)
{}

VideoCapture::~VideoCapture()
{
    close();
}

result VideoCapture::open(VideoConfig config)
{
    _config = config;
    R_ASSERT(_device_opened == Yes);

    LOG_INFO("VideoCapture opening with options:\n"
             "\tdevice_name: %s\n"
             "\tmax_buffers: %d\n"
             "\tframe_width: %d\n"
             "\tframe_height: %d\n"
             "\tformat: %d",
             _config.device_name.c_str(),
             _config.max_buffers,
             _config.frame_width,
             _config.frame_height,
             _get_vformat(_config.format));

    R_ASSERT_ERRNO((_cam_device = ::open(_config.device_name.c_str(), O_RDWR)) < 0);

    _device_opened = Yes;
    __u32 config_pixelformat = _get_vformat(_config.format);

    struct v4l2_format format;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = config_pixelformat;
    format.fmt.pix.width = _config.frame_width;
    format.fmt.pix.height = _config.frame_height;
    format.fmt.pix.field = V4L2_FIELD_ANY;

    R_BEGIN_ASSERT_ERRNO(ioctl(_cam_device, VIDIOC_S_FMT, &format) < 0);
    close();
    R_END_ASSERT;

    R_BEGIN_ASSERT_ERRNO(ioctl(_cam_device, VIDIOC_G_FMT, &format) < 0);
    close();
    R_END_ASSERT;

    LOG_INFO("Real cammera format:\n"
             "\tframe_width: %d\n"
             "\tframe_height: %d\n"
             "\tformat: %d",
             format.fmt.pix.width,
             format.fmt.pix.height,
             format.fmt.pix.pixelformat);

    R_BEGIN_ASSERT(format.fmt.pix.width != _config.frame_width ||
       format.fmt.pix.height != _config.frame_height);
    close();
    R_END_ASSERT;

    struct v4l2_requestbuffers bufrequest;
    memset(&bufrequest, 0, sizeof(v4l2_requestbuffers));
    bufrequest.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufrequest.memory = V4L2_MEMORY_MMAP;
    bufrequest.count = _config.max_buffers;

    R_BEGIN_ASSERT_ERRNO(ioctl(_cam_device, VIDIOC_REQBUFS, &bufrequest) < 0);
    close();
    R_END_ASSERT;

    R_BEGIN_ASSERT_ERRNO(bufrequest.count != _config.max_buffers);
    close();
    R_END_ASSERT;

    for(int bindex=0; bindex < _config.max_buffers; bindex++)
    {
        struct v4l2_buffer bufferInfo;
        memset(&bufferInfo, 0, sizeof(v4l2_buffer));

        bufferInfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufferInfo.memory = V4L2_MEMORY_MMAP;
        bufferInfo.index = bindex;

        R_BEGIN_ASSERT_ERRNO(ioctl(_cam_device, VIDIOC_QUERYBUF, &bufferInfo) < 0);
        close();
        R_END_ASSERT;

        VideoBuffer * buffer = new VideoBuffer();
        _video_buffers.push_back(buffer);

        R_BEGIN_ASSERT(!buffer->map(bufferInfo.length, _cam_device, bufferInfo.m.offset));
        close();
        R_END_ASSERT;
    }

    LOG_INFO("Camera device '%s' inited properly.", _config.device_name.c_str());

    return Success;
}

result VideoCapture::enable()
{
    R_ASSERT(_should_streaming == Yes || _is_streaming == Yes);
    R_ASSERT(_capture_thread != 0);

    _should_streaming = Yes;

    R_ASSERT_ERRNO(pthread_create(&_capture_thread, 0, VideoCapture::_static_thread_entry, this) < 0);

    return Success;
}

void VideoCapture::disable()
{
    if(_is_streaming == No && _capture_thread == 0)
        return;

    _should_streaming = No;

    if(_capture_thread != 0)
    {
        void * ret;
        timespec time_spec;
        clock_gettime(CLOCK_REALTIME,&time_spec);
        time_spec.tv_sec += 5;

        if(pthread_timedjoin_np(_capture_thread,&ret,&time_spec) != 0)
            LOG_WARNING("Couldn't stop capture thread within 5 sec.");

        _capture_thread = 0;
    }

    return;
}

void VideoCapture::close()
{
    LOG_INFO("Closing video device");

    disable();

    for (VideoBuffer* buffer : _video_buffers)
    {
        if(buffer != nullptr)
        {
            buffer->unmap();
            delete buffer;
        }
    }

    _video_buffers.clear();

    if(_device_opened == Yes)
        ::close(_cam_device);

    _device_opened = No;
}

result VideoCapture::_enable_stream()
{
    for(uint16_t bindex=0; bindex < _config.max_buffers; bindex++)
    {
        R_BEGIN_ASSERT(_queueBuffer(bindex) == Error);
        close();
        R_END_ASSERT;
    }

    int buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    R_ASSERT_ERRNO(ioctl(_cam_device, VIDIOC_STREAMON, &buf_type) < 0);

    _is_streaming = Yes;
    return Success;
}

result VideoCapture::_disable_stream()
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    R_ASSERT_ERRNO(ioctl(_cam_device, VIDIOC_STREAMOFF, &type) == -1);
    _is_streaming = No;
    return Success;
}

result VideoCapture::_next_frame()
{
    struct v4l2_buffer bufferInfo;
    memset(&bufferInfo, 0, sizeof(v4l2_buffer));

    bufferInfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferInfo.memory = V4L2_MEMORY_MMAP;

    R_ASSERT_ERRNO(ioctl(_cam_device, VIDIOC_DQBUF, &bufferInfo) == -1);
    if( _config.frame_handler(_video_buffers[bufferInfo.index]->buffer, bufferInfo.bytesused) == Error)
    {
        LOG_WARNING("Frame handling failed. Video capture will be disabled.");
        return Error;
    }

    R_ASSERT_ERRNO(ioctl(_cam_device, VIDIOC_QBUF, &bufferInfo) == -1);

    return Success;
}

result VideoCapture::_queueBuffer(uint16_t index)
{
    struct v4l2_buffer bufferInfo;
    memset(&bufferInfo, 0, sizeof(v4l2_buffer));

    bufferInfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferInfo.memory = V4L2_MEMORY_MMAP;
    bufferInfo.index = index;

    R_ASSERT_ERRNO(ioctl(_cam_device, VIDIOC_QBUF, &bufferInfo) < 0);

    return Success;
}

void VideoCapture::_thread_entry()
{
    LOG_INFO("Entering capture thread.");

    BEGIN_ASSERT(_enable_stream() == Error);
    _is_streaming = No;
    _should_streaming = No;
    END_ASSERT;

    while(_should_streaming == Yes &&
          _next_frame() != Error)
    {}

    ASSERT(_disable_stream() == Error);
    LOG_INFO("Returing from capture thread.");
}

void * VideoCapture::_static_thread_entry(void * self)
{
    ((VideoCapture*)self)->_thread_entry();
    return 0;
}

__u32 _get_vformat(video_format format)
{
    switch(format)
    {
    case video_format::YUV420M:
        return V4L2_PIX_FMT_YUV420M;
//    case video_format::YUV422M:
//        return V4L2_PIX_FMT_YVU422M;
    case video_format::H264:
        return V4L2_PIX_FMT_H264;
    case video_format::MJPEG:
        return V4L2_PIX_FMT_MJPEG;
    default:
        LOG_ERROR("Unsuported video format.");
        return -1;
    }
}
