# pywebrtc

**Description**: a python wrapper around the native webRTC API.

## TODO list

1. Compile [libwebrtc](https://github.com/jremmons/pywebrtc.git)

2. Write a basic C++ program to send fake webcam data to
[appr.tc](https://appr.tc) using the libwebrtc library.

3. Write C++ code to read/write from a reliable RTC data
channel. (also look into unreliable channels)

4. Write a python extension modules that provides a simple interface
to the unlying C++ code for specifying the source of video data.

5. Enhance the pyton extension module so it provides `read` and
`write` functions for communicating on the RTC data channel.