#include <iostream>
#include <string>
#include <thread>
#include <cstdlib>
#include <cstddef>
#include <mutex>

//#define WEBRTC_ANDROID 1
//#define WEBRTC_IOS 1
#define WEBRTC_LINUX 1
//#define WEBRTC_MAC 1
#define WEBRTC_POSIX 1
//#define WEBRTC_WIN 1

// WebRTC Dependencies
#include <webrtc/api/mediastreaminterface.h>
#include <webrtc/api/peerconnectioninterface.h>
#include <webrtc/base/ssladapter.h>
#include <webrtc/base/thread.h>
#include <webrtc/base/flags.h>
#include <webrtc/base/physicalsocketserver.h>

// JSON Parser Dependencies
#include "picojson/picojson.h"

#include <unistd.h>
// Refer to the API at: https://webrtc.googlesource.com/src/+/master/api/peerconnectioninterface.h

// Implementation for peer to peer connection observers (which manage callbacks)
class Connection {
  public:
    // Peer Connection
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;
    // Data Channel
    rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel;
    // SDP 
    std::string sdp_type;
    // ICE Information
    picojson::array ice_array;

    //websocket::stream<<ssl::stream<tcp::socket>>* ws;

    /*void initWebSocket(websocket::stream<ssl::stream<tcp::socket>>& ws_) {
        ws = ws_;
    }*/

    // On session success, set local description and send information to remote
    void sessionSuccess(webrtc::SessionDescriptionInterface* desc) {
      peer_connection->SetLocalDescription(ssdo, desc);
      picojson::object information;

      std::string sdp;
      desc->ToString(&sdp);
      information.insert(std::make_pair("sdp", picojson::value(sdp)));
      information.insert(std::make_pair("type", picojson::value(sdp_type)));
      // TODO: Figure out how to send to signaling server
	  std::cout << sdp << std::endl;
    }

    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
      std::cout << "On ICE Candidate" << std::endl;
      picojson::object ice;
      std::string candidate_str;
      candidate->ToString(&candidate_str);
      ice.insert(std::make_pair("candidate", picojson::value(candidate_str)));
      ice.insert(std::make_pair("sdpMid", picojson::value(candidate->sdp_mid())));
      ice.insert(std::make_pair("sdpMLineIndex", picojson::value(static_cast<double>(candidate->sdp_mline_index()))));
      ice_array.push_back(picojson::value(ice));
    }

    // Used to receive callbacks from the PeerConnection
    class PeerConnectionObs : public webrtc::PeerConnectionObserver {
      private:
        Connection &parent;

      public:
        PeerConnectionObs(Connection& parent) : parent(parent) {};

        void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override {
          std::cout << "PeerConnectionObserver Signaling Change" << std::endl;
        };

        void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
          std::cout << "PeerConnectionObserver Add Stream" << std::endl;
        };

        void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
          std::cout << "PeerConnectionObserver Remove Stream" << std::endl;
        };

        void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override {
          std::cout << "PeerConnectionObserver On Data Channel(" << data_channel << std::endl;
          parent.data_channel = data_channel;
          parent.data_channel->RegisterObserver(&parent.dco);
        };

        void OnRenegotiationNeeded() override {
          std::cout << "PeerConnectionObserver On Renegotiation Needed" << std::endl;
        };

        void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override {
          std::cout << "PeerConnectionObserver On Ice Connection Change" << std::endl;
        };

        void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override {
          std::cout << "PeerConnectionObserver On Ice Gathering Change" << std::endl;
        };

        void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override {
          std::cout << "PeerConnectionObserver On Ice Candidate" << std::endl;
        };
    };

    // Used to receive callbacks from the Data Channel 
    class DataChannelObs : public webrtc::DataChannelObserver {
      private:
        Connection &parent;
      public:
        DataChannelObs(Connection& parent) : parent(parent) {};

        void OnStateChange() override {
          std::cout << "DataChannelObserver On State Change" << std::endl;
        };

        void OnMessage(const webrtc::DataBuffer& buffer) override {
          std::cout << "DataChannelObserver On Message" << std::endl;
          std::cout << std::string(buffer.data.data<char>(), buffer.data.size()) << std::endl;
        };

        void OnBufferedAmountChange(uint64_t previous_amount) override {
          std::cout << "DataChannelObserver On Buffered Amount Change: " << previous_amount << std::endl;
        };
    };

    // Used to receive callbacks from creating the session description
    class CreateSessionDescriptionObs : public webrtc::CreateSessionDescriptionObserver {
      private:
        Connection &parent;
      public:
        CreateSessionDescriptionObs(Connection& parent) : parent(parent) {};

        void OnSuccess(webrtc::SessionDescriptionInterface* desc) override {
          std::cout << "CreateSessionDescriptionObserver Success Callback" << std::endl;
          parent.sessionSuccess(desc);
        };	

        void OnFailure(const std::string& error) override {
          std::cout << "CreateSessionDescriptionObserver Failure Callback. Error: " << error << std::endl;
        };
    };

    // Used to receive callbacks from setting the session description (not really used but necessary as a parameter)
    class SetSessionDescriptionObs : public webrtc::SetSessionDescriptionObserver {
      private:
        Connection &parent;
      public:
        SetSessionDescriptionObs(Connection& parent) : parent(parent) {};

        void OnSuccess() override {
          std::cout << "SetSessionDescriptionObserver Success Callback" << std::endl;
        };	

        void OnFailure(const std::string& error) override {
          std::cout << "SetSessionDescriptionObserver Failure Callback" << std::endl;
        };

    };

    PeerConnectionObs pco;
    DataChannelObs dco;
    rtc::scoped_refptr<CreateSessionDescriptionObs> csdo;
    rtc::scoped_refptr<SetSessionDescriptionObs> ssdo;

    Connection() :
      pco(*this),
      dco(*this),
      csdo(new rtc::RefCountedObject<CreateSessionDescriptionObs>(*this)),
      ssdo(new rtc::RefCountedObject<SetSessionDescriptionObs>(*this)) {}
};

// Variables needed for the implementation
std::unique_ptr<rtc::Thread> thread;
rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory;
webrtc::PeerConnectionInterface::RTCConfiguration configuration;
Connection connection;
rtc::PhysicalSocketServer socket_server;
rtc::scoped_refptr<webrtc::DataChannelInterface> channel;
std::mutex peer_connection_factory_mutex;

//typedef websocket::stream<<ssl::stream<tcp::socket>> ssl_stream;

class CustomRunnable : public rtc::Runnable {
  public:
    void Run(rtc::Thread* subthread) override {
      peer_connection_factory = webrtc::CreatePeerConnectionFactory();
      peer_connection_factory_mutex.unlock();
      if (peer_connection_factory.get() == nullptr) {
        std::cout << "Error on CreatePeerConnectionFactory." << std::endl;
        return;
      }

      subthread->Run();
    }
};


void createPeerConnection() {
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

/** 
	callerOffer: establishes the data channel to communicate and
				 then creates an offer to the callee
	@param nothing
	@return nothing
*/
void callerOffer() {

  // TODO: Swap out for video stream
  webrtc::DataChannelInit config;
  connection.data_channel = connection.peer_connection->CreateDataChannel("data_channel", &config);
  connection.data_channel->RegisterObserver(&connection.dco);

  connection.sdp_type = "offer"; 
  connection.peer_connection->CreateOffer(connection.csdo, nullptr);
}


void callerSetRemoteDescription(const std::string& parameter) {
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



// TODO: We are getting a json object as an offer from the web socket, figure out how to extract the sdp from it using picojson
/** 
	calleeAnswer: Function sets the remote description of the caller
				  and then creates an answer for the caller
	@param parameter: the string offer sent by the caller
	@return  nothing
*/
void calleeAnswer(const std::string& parameter) {
  webrtc::SdpParseError error;
  webrtc::SessionDescriptionInterface* session_description(
      webrtc::CreateSessionDescription("offer", parameter, &error));

  if (session_description == nullptr) {
    std::cout << "Error on CreateSessionDescription." << std::endl
      << error.line << std::endl
      << error.description << std::endl;
  }

  connection.peer_connection->SetRemoteDescription(connection.ssdo, session_description);

  connection.sdp_type = "answer";
  connection.peer_connection->CreateAnswer(connection.csdo, nullptr);
}

//TODO: Figure out how to send this to the signaling server?
void sendICEInformation() {
  std::cout << picojson::value(connection.ice_array).serialize(true) << std::endl;
  connection.ice_array.clear();
}

void setICEInformation(const std::string& parameter) {
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

void cmd_sdp1() {
  connection.peer_connection = peer_connection_factory->CreatePeerConnection(configuration, nullptr, nullptr, &connection.pco);

  webrtc::DataChannelInit config;
  // DataChannelの設定

  connection.data_channel = connection.peer_connection->CreateDataChannel("data_channel", &config);
  connection.data_channel->RegisterObserver(&connection.dco);

  if (connection.peer_connection.get() == nullptr) {
    peer_connection_factory = nullptr;
    std::cout << "Error on CreatePeerConnection." << std::endl;
    return;
  }
  connection.sdp_type = "Offer"; // 表示用の文字列、webrtcの動作には関係ない
  connection.peer_connection->CreateOffer(connection.csdo, nullptr);
}

void disconnectFromCurrentPeer() {
  // TODO: Send message to other peer to disconnect
  connection.peer_connection->Close();
  connection.peer_connection = nullptr;
  connection.data_channel = nullptr;
  peer_connection_factory = nullptr;

  thread->Quit();
}
 
int main(int argc, char **argv) {
  rtc::FlagList::SetFlagsFromCommandLine(&argc, argv, true);
  rtc::FlagList::Print(nullptr, false);

  thread.reset(new rtc::Thread(&socket_server));

  // Initialize ssl and thread manager
  rtc::InitializeSSL();

  // 1. Create a PeerConnectionFactoryInterface
  peer_connection_factory_mutex.lock();
  CustomRunnable runnable;
  thread->Start(&runnable);
  


  // 2. Set up web socket connection to signaling server
  auto const host = "ccr-frontend-0.jemmons.us";
  auto const port = "443";
  auto const path = "ccr";


	// 2. Create a PeerConnection object with configuration and PeerConnectionObserver
  //createPeerConnection();
  //callerOffer();
  while(!peer_connection_factory_mutex.try_lock()) {}
  peer_connection_factory_mutex.unlock();

  cmd_sdp1();

  disconnectFromCurrentPeer();
  thread.reset();
  rtc::CleanupSSL();

  return 0;
}
