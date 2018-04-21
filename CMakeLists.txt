cmake_minimum_required(VERSION 3.3)
project(pywebrtc)

set(CMAKE_BUILD_TYPE Debug)

# get LibWebRTC
find_package(LibWebRTC REQUIRED)
include(${LIBWEBRTC_USE_FILE})

# get OpenSSL
find_package(OpenSSL)

# get Boost
set(BOOST_ROOT /usr/local)
find_package(Boost COMPONENTS system REQUIRED) 
include_directories(${Boost_INCLUDE_DIRS}) 
include_directories(${OPENSSL_INCLUDE_DIRS})

# create websocket executable
add_executable(websocket websocket_client_sync.cpp)

# link websocket libraries
target_link_libraries(websocket ${Boost_LIBRARIES})
target_link_libraries(websocket ${OPENSSL_LIBRARIES})
# target_link_libraries(pywebrtc ${LIBWEBRTC_LIBRARIES})

# add source files
set(SOURCE_FILES main.cc)

# create pywebrtc executable
add_executable(pywebrtc ${SOURCE_FILES})

# link pywebrtc libraries
target_link_libraries(pywebrtc ${Boost_LIBRARIES})
target_link_libraries(pywebrtc ${OPENSSL_LIBRARIES})
target_link_libraries(pywebrtc ${LIBWEBRTC_LIBRARIES})

# create websocket executable
add_executable(websocket-offer websocket_offer.cpp)

# link websocket libraries
target_link_libraries(websocket-offer ${Boost_LIBRARIES})
target_link_libraries(websocket-offer ${OPENSSL_LIBRARIES})

# create sample executable
add_executable(sample sample.cc)

# link pywebrtc libraries
target_link_libraries(sample ${Boost_LIBRARIES})
target_link_libraries(sample ${OPENSSL_LIBRARIES})
target_link_libraries(sample ${LIBWEBRTC_LIBRARIES})

