#include <string>

#include <webrtc/api/mediastreaminterface.h>
#include <webrtc/api/peerconnectioninterface.h>
#include <webrtc/base/ssladapter.h>
#include <webrtc/base/thread.h>
#include <webrtc/base/flags.h>
#include <webrtc/base/physicalsocketserver.h>

#include "libwebrtc_hl.hh"
#include "connection.hh"

extern const size_t WebRTCConnectionSize = sizeof(LibWebRTC::WebRTCConnection);

LibWebRTC::WebRTCConnection::WebRTCConnection(std::string kind) :
  connection(),
  runnable(peer_connection_factory, peer_connection_factory_mutex)
{

  //rtc::FlagList::SetFlagsFromCommandLine(&zero, nullptr, true);
  //rtc::FlagList::Print(nullptr, false);

  thread.reset(new rtc::Thread(&socket_server));

  // Initialize ssl and thread manager
  rtc::InitializeSSL();

  // 1. Create a PeerConnectionFactoryInterface
  peer_connection_factory_mutex.lock();
  thread->Start(&runnable);
  
  // 2. Create a PeerConnection object with configuration and PeerConnectionObserver
  //callerOffer();
  while(!peer_connection_factory_mutex.try_lock()) {}
  peer_connection_factory_mutex.unlock();

  createPeerConnection();
  
}

LibWebRTC::WebRTCConnection::~WebRTCConnection(void) {

  disconnectFromCurrentPeer();
  thread.reset();
  rtc::CleanupSSL();

}

void LibWebRTC::WebRTCConnection::createPeerConnection() {
  //connection = new Connection(ws);
  webrtc::PeerConnectionInterface::RTCConfiguration config;

  // STUN servers: https://github.com/jremmons/ccr-web/blob/master/static/js/app.js
  webrtc::PeerConnectionInterface::IceServer firstServer;	
  webrtc::PeerConnectionInterface::IceServer secondServer;
  webrtc::PeerConnectionInterface::IceServer thirdServer;

  firstServer.uri = "stun:35.197.92.183:3478";
  secondServer.uri = "turn:35.197.92.183:3479?transport=udp";
  secondServer.username = "ccrturnserver";
  secondServer.password = "turn";
  thirdServer.uri = "turn:35.197.92.183:3479?transport=tcp";
  thirdServer.username = "ccrturnserver";
  thirdServer.password = "turn";

  configuration.servers.push_back(firstServer);
  configuration.servers.push_back(secondServer);
  configuration.servers.push_back(thirdServer);

  connection.peer_connection = peer_connection_factory->CreatePeerConnection(configuration, nullptr, nullptr, &connection.pco);


  if (connection.peer_connection.get() == nullptr) {
    peer_connection_factory = nullptr;
    std::cout << "Error on CreatePeerConnection." << std::endl;
    return;
  }

}

void LibWebRTC::WebRTCConnection::disconnectFromCurrentPeer(void) {

  // TODO: Send message to other peer to disconnect
  connection.peer_connection->Close();
  connection.peer_connection = nullptr;
  connection.data_channel = nullptr;
  peer_connection_factory = nullptr;

  thread->Quit();

}

std::string LibWebRTC::WebRTCConnection::get_offer(void) {

  webrtc::DataChannelInit config;
  connection.data_channel = connection.peer_connection->CreateDataChannel("data_channel", &config);
  connection.data_channel->RegisterObserver(&connection.dco);

  connection.sdp_type = "offer"; 
  connection.peer_connection->CreateOffer(connection.csdo, nullptr);

  std::string offer = connection.get_sdp();
  
  return offer;
}
