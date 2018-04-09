cmake_minimum_required(VERSION 3.3)
project(pywebrtc)

find_package(LibWebRTC REQUIRED)
include(${LIBWEBRTC_USE_FILE})

target_link_libraries(my-app ${LIBWEBRTC_LIBRARIES})

set(SOURCE_FILES rtc.cc)
add_executable(rtc ${SOURCE_FILES})
