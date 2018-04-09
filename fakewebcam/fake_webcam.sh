#!/bin/sh

ffmpeg -i demo.mp4 -f v4l2 /dev/video0
