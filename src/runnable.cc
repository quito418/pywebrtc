#include <iostream>
#include "runnable.hh"

CustomRunnable::CustomRunnable(std::mutex *m) :
  peer_connection_factory_mutex_(m),
  peer_connection_factory(nullptr)
{}
      
void CustomRunnable::Run(rtc::Thread* subthread) {

  try {
    //peer_connection_factory = webrtc::CreatePeerConnectionFactory();
  }
  catch(...){
    std::cerr << "could not create peer_connection_factory\n";
  }
  
  peer_connection_factory_mutex_->unlock();
  if (peer_connection_factory == nullptr) {
    std::cout << "Error on CreatePeerConnectionFactory." << std::endl;
    return;
  }
  
  subthread->Run();
}
