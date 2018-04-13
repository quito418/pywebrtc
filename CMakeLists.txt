cmake_minimum_required(VERSION 3.3)
project(pywebrtc)

find_package(LibWebRTC REQUIRED)
include(${LIBWEBRTC_USE_FILE})

find_package(Boost COMPONENTS system REQUIRED)

# Use the package PkgConfig to detect GTK+ headers/library files
#FIND_PACKAGE(PkgConfig REQUIRED)
#PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
#INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
#LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})

# Add other flags to the compiler
#ADD_DEFINITIONS(${GTK3_CFLAGS_OTHER})


#set(SOURCE_FILES main_example.cc conductor.cc)
set(SOURCE_FILES main.cc conductor.cc)
#set(SOURCE_FILES examples/peerconnection/client/linux/main.cc)
add_executable(pywebrtc ${SOURCE_FILES})

target_link_libraries(pywebrtc ${LIBWEBRTC_LIBRARIES})

# Link the target to the GTK+ libraries
#TARGET_LINK_LIBRARIES(pywebrtc ${GTK3_LIBRARIES})

add_executable(websocket websocket_client_sync.cpp)
