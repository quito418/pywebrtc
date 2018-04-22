#pragma once

#include <webrtc/api/peerconnectioninterface.h>
#include <mutex>

class CustomRunnable : public rtc::Runnable {
private:
  std::mutex *peer_connection_factory_mutex_; 
  webrtc::PeerConnectionFactoryInterface **peer_connection_factory_;

public:
  CustomRunnable(std::mutex *m, webrtc::PeerConnectionFactoryInterface **f);
  void Run(rtc::Thread* subthread) override;

};
