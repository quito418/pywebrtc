#include <iostream>

#include "libwebrtc_hl.hh"

int main(int argc, char* argv[]) {
 
  std::cout << "constructing connection class" << std::endl;
  LibWebRTC::WebRTCConnection *conn;

  conn = new LibWebRTC::WebRTCConnection("server");
  
  std::cout << "getting offer" << std::endl;
  //std::cerr <<  conn->get_offer() << std::endl;
 
  std::string testOffer = "v=0\r\no=- 8315849958323870745 3 IN IP4 127.0.0.1\r\ns=-\r\nt=0 0\r\na=group:BUNDLE data\r\na=msid-semantic: WMS\r\nm=application 37948 DTLS\/SCTP 5000\r\nc=IN IP4 10.138.0.51\r\na=candidate:2496409815 1 udp 2122260223 10.138.0.51 37948 typ host generation 0 network-id 1 network-cost 50\r\na=ice-ufrag:4bJW\r\na=ice-pwd:dmlnBwXiJIhAdi6V6zwWLDPh\r\na=ice-options:trickle\r\na=fingerprint:sha-256 89:28:66:C2:06:A3:DA:4B:97:7B:F5:C3:A7:99:F5:1D:95:E0:D4:88:10:EE:DC:1F:C4:25:37:BD:D6:E8:A5:8A\r\na=setup:actpass\r\na=mid:data\r\na=sctpmap:5000 webrtc-datachannel 1024\r\n";
  std::cerr << conn->receiveOffer(testOffer) << std::endl;

  std::cout << "sending ice information" << std::endl;
  std::cerr << conn->getICEInformation() << std::endl;
  delete conn;
  
  return 0;
}
