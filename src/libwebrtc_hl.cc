#define WEBRTC_LINUX 1
#define WEBRTC_POSIX 1

#include <string>

#include <webrtc/api/peerconnectioninterface.h>
#include <webrtc/base/ssladapter.h>
#include <webrtc/base/thread.h>
#include <webrtc/base/flags.h>
#include <webrtc/base/physicalsocketserver.h>
#include <webrtc/media/engine/webrtcvideocapturerfactory.h>
#include <webrtc/modules/video_capture/video_capture_factory.h>
#include "webrtc/api/mediastreaminterface.h"
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

  //addStreams();
  
}

LibWebRTC::WebRTCConnection::~WebRTCConnection(void) {

  disconnectFromCurrentPeer();
  thread.reset();
  rtc::CleanupSSL();

}

void LibWebRTC::WebRTCConnection::addTracks(const std::string& deviceId) {
  if (!connection.peer_connection->GetSenders().empty()) {
    return;  // Already added tracks.
  }
 
  webrtc::MediaStreamInterface* stream =
      peer_connection_factory->CreateLocalMediaStream("stream_id");
  
  std::vector<webrtc::MediaStreamInterface*> streams;
  streams.push_back(stream);
  
  rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
      peer_connection_factory->CreateAudioTrack(
          "audio_label", peer_connection_factory->CreateAudioSource(nullptr)));

  
  auto result_or_error = connection.peer_connection->AddTrack(audio_track, streams);
 
  if (!result_or_error->track()) {
    std::cout << "Failed to add audio track to PeerConnection: " << std::endl;
  }

  std::unique_ptr<cricket::VideoCapturer> video_device =
      OpenVideoCaptureDevice(deviceId);
  if (video_device) {
    rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track_(
        peer_connection_factory->CreateVideoTrack(
            "video_label", peer_connection_factory->CreateVideoSource(
                             std::move(video_device), nullptr)));

    result_or_error = connection.peer_connection->AddTrack(video_track_, streams);
    if (!result_or_error->track()) {
      std::cout << "Failed to add video track to PeerConnection: " << std::endl;
    }
  }
  else {
    std::cout << "OpenVideoCaptureDevice failed" << std::endl;
  }

}

std::unique_ptr<cricket::VideoCapturer> LibWebRTC::WebRTCConnection::OpenVideoCaptureDevice(const std::string&deviceId) {
  std::vector<std::string> device_names;
  {
    std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(
        webrtc::VideoCaptureFactory::CreateDeviceInfo());
    if (!info) {
      std::cout << "Unable to find any devices" << std::endl;
      return nullptr;
    }
    int num_devices = info->NumberOfDevices();
    for (int i = 0; i < num_devices; ++i) {
      const uint32_t kSize = 256;
      char name[kSize] = {0};
      char id[kSize] = {0};
      if (info->GetDeviceName(i, name, kSize, id, kSize) != -1) {
        if(id == deviceId) {
          std::cout << "Found video device" << std::endl;
          device_names.push_back(name);
        }
      }
    }
  }

  cricket::WebRtcVideoDeviceCapturerFactory factory;
  std::unique_ptr<cricket::VideoCapturer> capturer;
  for (const auto& name : device_names) {
    std::cout << "Create Factory for device: " << name << std::endl;
    capturer = factory.Create(cricket::Device(name, 0));
    if (capturer) {
      break;
    }
  }
  return capturer;
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
  active_streams_.clear();
  connection.peer_connection->Close();
  connection.peer_connection = nullptr;
  connection.data_channel = nullptr;
  peer_connection_factory = nullptr;

  thread->Quit();

}

std::string LibWebRTC::WebRTCConnection::get_offer(void) {

  webrtc::DataChannelInit config;
  //connection.data_channel = connection.peer_connection->CreateDataChannel("data_channel", &config);
  //connection.data_channel->RegisterObserver(&connection.dco);

  std::cout << "IN GET_OFFER ***************************" <<std::endl ;
  connection.sdp_type = "offer"; 
  connection.peer_connection->CreateOffer(connection.csdo, nullptr);

  //connection.peer_connection->setLocalDescription(connection.ssdo, nullptr);
  std::string offer = connection.get_sdp();

  

  return offer;
}

void LibWebRTC::WebRTCConnection::receiveAnswer(const std::string& parameter) {
  webrtc::SdpParseError error;
  webrtc::SessionDescriptionInterface* session_description(
      webrtc::CreateSessionDescription("answer", parameter, &error));
  if (session_description == nullptr) {
    std::cout << "Error on CreateSessionDescription." << std::endl
      << error.line << std::endl
      << error.description << std::endl;
  }
  connection.peer_connection->SetRemoteDescription(connection.ssdo, session_description);
}

std::string LibWebRTC::WebRTCConnection::receiveOffer(const std::string& parameter) {
  webrtc::SdpParseError error;
  webrtc::SessionDescriptionInterface* session_description(
      webrtc::CreateSessionDescription("offer", parameter, &error));

  if (session_description == nullptr) {
    std::cout << "Error on CreateSessionDescription." << std::endl
      << error.line << std::endl
      << error.description << std::endl;
  }

  connection.peer_connection->SetRemoteDescription(connection.ssdo, session_description);

  webrtc::DataChannelInit config;
  //connection.data_channel = connection.peer_connection->CreateDataChannel("data_channel", &config);
  //connection.data_channel->RegisterObserver(&connection.dco);

  connection.sdp_type = "answer";
  connection.peer_connection->CreateAnswer(connection.csdo, nullptr);

  std::string answer = connection.get_sdp();

  return answer;
}

std::string LibWebRTC::WebRTCConnection::getICEInformation(void) {
  std::string connectionICE = picojson::value(connection.ice_array).serialize(true);
  connection.ice_array.clear();
  return connectionICE;
}

void LibWebRTC::WebRTCConnection::setICEInformation(const std::string& parameter) {
  picojson::value v;
  std::string err = picojson::parse(v, parameter);
  if (!err.empty()) {
    std::cout << "Error on parse json : " << err << std::endl;
    return;
  }

  webrtc::SdpParseError err_sdp;
  for (auto& ice_it : v.get<picojson::array>()) {
    picojson::object& ice_json = ice_it.get<picojson::object>();
    webrtc::IceCandidateInterface* ice =
      CreateIceCandidate(ice_json.at("sdpMid").get<std::string>(),
          static_cast<int>(ice_json.at("sdpMLineIndex").get<double>()),
          ice_json.at("candidate").get<std::string>(),
          &err_sdp);
    if (!err_sdp.line.empty() && !err_sdp.description.empty()) {
      std::cout << "Error on CreateIceCandidate" << std::endl
        << err_sdp.line << std::endl
        << err_sdp.description << std::endl;
      return;
    }
    connection.peer_connection->AddIceCandidate(ice);
  }
}

void LibWebRTC::WebRTCConnection::sendString(const std::string& parameter) {
  webrtc::DataBuffer buffer(rtc::CopyOnWriteBuffer(parameter.c_str(), parameter.size()), true);
  std::cout << "Send '" << parameter << "': state(" << connection.data_channel->state() << ")" << std::endl;
  connection.data_channel->Send(buffer);
}

bool LibWebRTC::WebRTCConnection::dataChannelOpen() {
  return connection.data_channel_open.load();
}

bool LibWebRTC::WebRTCConnection::videoStreamOpen() {
  return connection.video_stream_open.load();
}

bool LibWebRTC::WebRTCConnection::peerConnectionFailed() {
  return connection.peer_connection_failed.load();
}

std::vector<std::string> LibWebRTC::WebRTCConnection::dataBuffer() {
  return connection.data_buffer;
}

void LibWebRTC::WebRTCConnection::clearDataBuffer() {
  connection.data_buffer.clear();
}
