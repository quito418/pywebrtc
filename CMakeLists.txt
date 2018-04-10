cmake_minimum_required(VERSION 3.3)
project(pywebrtc)

find_package(LibWebRTC REQUIRED)
include(${LIBWEBRTC_USE_FILE})

set(SOURCE_FILES main.cc)
add_executable(pywebrtc ${SOURCE_FILES})

target_link_libraries(pywebrtc ${LIBWEBRTC_LIBRARIES})

