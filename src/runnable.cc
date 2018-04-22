#include <iostream>
#include <chrono>
#include <thread>

#include "runnable.hh"

CustomRunnable::CustomRunnable(std::mutex *m, webrtc::PeerConnectionFactoryInterface **f) :
  rtc::Runnable(),
  peer_connection_factory_mutex_(m),
  peer_connection_factory_(f)
{}
      
void CustomRunnable::Run(rtc::Thread* subthread) {

  try {
    //std::cerr << "!!! creating peer_connection_factory !!!" << std::endl;
    *peer_connection_factory_ = webrtc::CreatePeerConnectionFactory();
  }
  catch(...){
    std::cerr << "could not create peer_connection_factory\n";
  }
  
  peer_connection_factory_mutex_->unlock();
  if (peer_connection_factory_ == nullptr or *peer_connection_factory_ == nullptr) {
    std::cout << "Error on CreatePeerConnectionFactory." << std::endl;
    return;
  }
  
  subthread->Run();
}
