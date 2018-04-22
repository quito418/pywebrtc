#pragma once

#include <webrtc/api/peerconnectioninterface.h>
#include <mutex>

class CustomRunnable : public rtc::Runnable {
private:
  std::mutex *peer_connection_factory_mutex_; 
  webrtc::PeerConnectionFactoryInterface *peer_connection_factory;

public:
  //CustomRunnable(std::mutex *m, webrtc::PeerConnectionFactoryInterface *f);
  CustomRunnable(std::mutex *m);
  void Run(rtc::Thread* subthread) override;

};
