#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <mutex>

#include "connection.hh"
#include "runnable.hh"

namespace LibWebRTC {

  class WebRTCConnection {
  private:

    rtc::PhysicalSocketServer socket_server;

    Connection connection;

    CustomRunnable runnable;
    std::mutex peer_connection_factory_mutex;
    webrtc::PeerConnectionFactoryInterface *peer_connection_factory;

    std::unique_ptr<rtc::Thread> thread;
    webrtc::PeerConnectionInterface::RTCConfiguration configuration;
    rtc::scoped_refptr<webrtc::DataChannelInterface> channel;
    
    void createPeerConnection(void);
    void disconnectFromCurrentPeer(void);    

  public:

    WebRTCConnection(std::string kind);
    ~WebRTCConnection(void);

    std::string get_offer(void);

  };
  

}
