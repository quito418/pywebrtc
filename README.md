# pywebrtc

**Description**: a python wrapper around the native webRTC API.

## How to build

```
# download the libwebrtc precompiled library (not necessary if libwebrtc is installed already)
# https://drive.google.com/file/d/1pRqARD-YQwXj12n1RgQMhS5E9Ega2ogH/view?usp=sharing
tar xf libwebrtc.tar

# set your CFLAGS and LDFLAGS so the code will compile
export CFLAGS=$(pwd)/libwebrtc/out/include
export LDFLAGS=$(pwd)/libwebrtc/out/lib
```

```bash
./autogen.sh
./configure

make clean # shouldn't be necessary, but run if you are paranoid
make -j
```

## How to run test

```bash
export PYTHONPATH=$(pwd)/python/build/lib.linux-x86_64-3.5/
export LD_LIBRARY_PATH=$(pwd)/src/.libs/

python test.py
```

## TODO list

1. Compile [libwebrtc](https://github.com/aisouard/libwebrtc)

2. Write a basic C++ program to send fake webcam data to
[appr.tc](https://appr.tc) using the libwebrtc library.

3. Write C++ code to read/write from a reliable RTC data
channel. (also look into unreliable channels)

4. Write a python extension modules that provides a simple interface
to the unlying C++ code for specifying the source of video data.

5. Enhance the pyton extension module so it provides `read` and
`write` functions for communicating on the RTC data channel.
