#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <vector>

#include "connection.hh"

namespace LibWebRTC {

  class WebRTCConnection {
  private:

    Connection connection;
    CustomRunnable runnable;

    std::unique_ptr<rtc::Thread> thread;
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory;
    webrtc::PeerConnectionInterface::RTCConfiguration configuration;
    rtc::PhysicalSocketServer socket_server;
    rtc::scoped_refptr<webrtc::DataChannelInterface> channel;
    std::mutex peer_connection_factory_mutex;

    void createPeerConnection(void);
    void disconnectFromCurrentPeer(void);    

  public:
    WebRTCConnection(std::string kind);
    ~WebRTCConnection(void);

    // Handshaking methods
    std::string get_offer(void);
    void receiveAnswer(const std::string& parameter);
    std::string receiveOffer(const std::string& parameter);
    std::string getICEInformation(void);
    void setICEInformation(const std::string& parameter);

    // Data transfer methods
    void sendString(const std::string& parameter);
    bool dataChannelOpen();
    std::vector<std::string> dataBuffer();
    void clearDataBuffer();
    
  };
  

}
