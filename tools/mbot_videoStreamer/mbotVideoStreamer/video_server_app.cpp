#include <csignal>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include <mbot/core/mbot_logger.h>
#include <mbot/core/mbot_args_parser.h>
#include <mbot/core/mbot_tcp_server.h>
#include <mbot/videoCapture/mbot_video_capture.h>

#include <thread>

mbot::TcpServer tcp_server;
mbot::video::VideoCapture video_capture;

int should_run = 1;
int tcp_client_fd = -1;

mbot::video::video_format str2fmt(std::string fmt);
void signal_handler(int s);
mbot::result on_frame(void * data, uint32_t size);

bool raw_sreaming = false;

int main(int argc, char ** argv)
{
    mbot::ArgsParser parser(argc, argv);
    parser.addParam("port","Listening port.", true);
    parser.addParam("vdevice","Video device. (e.g. /dev/video0)", true);
    parser.addParam("bcount","Buffers count. (e.g. 3)", true);
    parser.addParam("frame_width","Video frame width. (e.g. 800)", true);
    parser.addParam("frame_height","Video frame height. (e.g. 600)", true);
    parser.addParam("frame_fmt","Video frame format. [YUV420M, YUV422M, H264, MJPEG]", true);
    parser.addParam("raw_stream","Enable raw video streaming (without extra frame info). [def 0, 1]", true);
    parser.addParam("log_level","Logging level. [def DEBUB, INFO]", false);
    parser.addParam("log_std","Enable logging to stdout. [def 0, 1]", false);

    if(parser.parse() == mbot::Error)
    {
        parser.printHelp();
        exit(1);
    }

    mbot::Logger::init(
        mbot::LoggerConfig("VIDEO_SERVICE",
                                   parser.readString("log_level") == "INFO" ?
                                   mbot::LOG_LEVEL::INFO :
                                   mbot::LOG_LEVEL::DEBUG,
                                   parser.readInt("log_std") == 1));

    raw_sreaming = parser.readInt("raw_stream") == 1;

    if(raw_sreaming)
        LOG_INFO("Raw streaming enabled.");
    else
        LOG_INFO("Raw streaming disabled.");

    std::signal(SIGPIPE, SIG_IGN);
    std::signal (SIGINT, signal_handler);

    if(tcp_server.start(parser.readInt("port")) == mbot::Error)
      return 1;

    if(video_capture.open(mbot::video::VideoConfig(parser.readString("vdevice"),
                                                    parser.readInt("bcount"),
                                                    parser.readInt("frame_width"),
                                                    parser.readInt("frame_height"),
                                                    str2fmt(parser.readString("frame_fmt")),
                                                    &on_frame)) == mbot::Error)
      return 1;


    while(should_run)
    {
        int new_client = tcp_server.accept();
        if(tcp_client_fd != -1)
        {
            video_capture.disable();
            close(tcp_client_fd);
        }

        if(should_run)
        {
            tcp_client_fd = new_client;
            if(video_capture.enable() == mbot::Error)
                return 1;
        }
    }
    return 0;
}

mbot::video::video_format str2fmt(std::string fmt)
{
//    if(fmt == "YUV422M")
//        return mbot::video::video_format::YUV422M;

    if(fmt == "YUV420M")
        return mbot::video::video_format::YUV420M;

    if(fmt == "MJPEG")
        return mbot::video::video_format::MJPEG;

    if(fmt == "H264")
        return mbot::video::video_format::H264;

    LOG_ERROR("Invalid frame format: %s. Fallback to default: YUV422M", fmt.c_str());
    return mbot::video::video_format::YUV420M;

}

void signal_handler(int s)
{
    LOG_INFO("SIGINT received. Closing server.");
    should_run = false;
    video_capture.close();
    tcp_server.stop();
}

int meta_data_size = sizeof(uint32_t);
char * meta_data = new char[meta_data_size];

mbot::result on_frame(void * data, uint32_t size)
{
    if(!raw_sreaming)
    {
        memcpy(meta_data, (void*)&size, meta_data_size);
        if(send(tcp_client_fd, meta_data, meta_data_size, MSG_NOSIGNAL) == -1)
        {
            LOG_WARNING("Tcp client disconneced.");
            return mbot::Error;
        }
    }

    if(send(tcp_client_fd, data, size, MSG_NOSIGNAL) == -1)
    {
        LOG_WARNING("Tcp client disconneced.");
        return mbot::Error;
    }

    return mbot::Success;
}
