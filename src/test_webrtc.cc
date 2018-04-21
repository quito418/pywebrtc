#include <iostream>

#include "libwebrtc_hl.hh"

int main(int argc, char* argv[]) {
 
  std::cout << "constructing connection class" << std::endl;
  LibWebRTC::WebRTCConnection conn("server");

  std::cout << "getting offer" << std::endl;
  std::cerr <<  conn.get_offer() << std::endl;

  return 0;
}
