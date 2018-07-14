#!/bin/bash

git pull origin master

mkdir -p ./libs/mbot_core/build/
cd ./libs/mbot_core/build/
cmake ../mbotCore/
sudo make install

cd ../../..

mkdir -p ./libs/mbot_video_capture/build/
cd ./libs/mbot_video_capture/build/
cmake ../mbotVideoCapture/
sudo make install

cd ../../..

mkdir -p ./tools/mbot_videoStreamer/build/
cd ./tools/mbot_videoStreamer/build
cmake ../mbotVideoStreamer/
sudo make install
