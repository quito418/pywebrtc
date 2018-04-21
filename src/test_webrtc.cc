#include <iostream>

#include "libwebrtc_hl.hh"

int main(int argc, char* argv[]) {
 
  std::cout << "hello world" << std::endl;

  auto conn = LibWebRTC::WebRTCConnection();
  conn.run(argc, argv);

  return 0;
}
