#include <iostream>
#include <string>

#include <webrtc/api/mediastreaminterface.h>
#include <webrtc/api/peerconnectioninterface.h>
#include <webrtc/base/ssladapter.h>
#include <webrtc/base/thread.h>
#include <webrtc/base/physicalsocketserver.h>
#include "picojson/picojson.h"

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

	// On session success, set local description and send information to remote
	void sessionSuccess(webrtc::SessionDescriptionInterface* desc) {
		peer_connection->SetLocalDescription(&set_session_description_observer, desc);
		// TODO: Figure out how to serialize (maybe stringify it or json it? then send to peer)
	}

	void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
		std::cout << "On ICE Candidate" << std::endl;
		picojosn::object ice;
		candidate->ToString(&candidate_str);
    	ice.insert(std::make_pair("candidate", picojson::value(candidate_str)));
    	ice.insert(std::make_pair("sdpMid", picojson::value(candidate->sdp_mid())));
    	ice.insert(std::make_pair("sdpMLineIndex", picojson::value(static_cast<double>(candidate->sdp_mline_index()))));
    	ice_array.push_back(picojson::value(ice));
	}

	// Used to receive callbacks from the PeerConnection
	class PeerConnectionObserver : public webrtc::PeerConnectionObserver {
		private:
			Connection &parent;

		public:
		PeerConnectionObserver(Connection& parent) : parent(parent) {};

		void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override {
			std::cout << "PeerConnectionObserver Signaling Change" << std::endl;
		};

		void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
			std::cout << "PeerConnectionObserver Add Stream" << std::endl;
		};
		
		void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface) {
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
	class DataChannelObserver : public webrtc::DataChannelObserver {
		private:
			Connection &parent;
		public
			DataChannelObserver(Connection& parent) : parent(parent) {};

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
	class CreateSessionDescriptionObserver : public webrtc::CreateSessionDescriptionObserver {
		private:
			Connection &parent;
		public
			CreateSessionDescriptionObserver(Connection& parent) : parent(parent) {};

		void OnSuccess(webrtc::SessionDescriptionInterface* desc) override {
			std::cout << "CreateSessionDescriptionObserver Success Callback" << std::endl;
			parent.sessionSuccess(desc);
		};	

		void OnFailure(const std::string& error) override {
			std::cout << "CreateSessionDescriptionObserver Failure Callback. Error: " << error << std::endl;
		};
	};

	// Used to receive callbacks from setting the session description (not really used but necessary as a parameter)
	class SetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver {
		private:
			Connection &parent;
		public
			SetSessionDescriptionObserver(Connection& parent) : parent(parent) {};

		void OnSuccess() override {
			std::cout << "SetSessionDescriptionObserver Success Callback" << std::endl;
		};	

		void OnFailure(const std::string& error) override {
			std::cout << "SetSessionDescriptionObserver Failure Callback" << std::endl;
		};

	};

	PeerConnectionObserver pco;
	DataConnectionObserver dco;
	rtc::scoped_refptr<CreateSessionDescriptionObserver> csdo;
	rtc::scoped_refptr<SetSessionDescriptionObserver> ssdo;

	Connection() :
		pco(*this),
		dco(*this),
		csdo(new rtc::RefCountedObject<CreateSessionDescriptionObserver>(*this)),
      	ssdo(new rtc::RefCountedObject<SetSessionDescriptionObserver>(*this)) {

}

rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory;
webrtc::PeerConnectionInterface::RTCConfiguration configuration;
Connection connection;
rtc::PhysicalSocketServer socket_server;

class CustomRunnable : public rtc::Runnable {
 public:
  void Run(rtc::Thread* subthread) override {
    peer_connection_factory = webrtc::CreatePeerConnectionFactory(
        webrtc::CreateBuiltinAudioEncoderFactory(),
        webrtc::CreateBuiltinAudioDecoderFactory());
    if (peer_connection_factory.get() == nullptr) {
      std::cout << "Error on CreatePeerConnectionFactory." << std::endl;
      return;
    }

    subthread->Run();
  }
};

bool createPeerConnection() {
	webrtc::PeerConnectionInterface::RTCConfiguration config;
	webrtc::PeerConnectionInterface::IceServer server;
	// TODO: Put our own STUN server in here later
	server.uri = "stun:stun.1.google.com:19302";
	configuration.servers.push_back(server);

	connection.peer_connection = peer_connection_factory->CreatePeerConnection(configuration, nullptr, nullptr, &connection.pco);
	
	// TODO: Swap out for video stream
	webrtc::DataChannel config;
	connection.data_channel = connection.peer_connection->CreateDataChannel("data_channel", &config);
	connection.data_channel->RegisterObserver(&connection.dco);

	if (connection.peer_connection.get() == nullptr) {
    	peer_connection_factory = nullptr;
    	std::cout << "Error on CreatePeerConnection." << std::endl;
    	return;
  	}

  	connection.sdp_type = "offer"; 
  	connection.peer_connection->CreateOffer(connection.csdo, nullptr);
}

int main(int argc, char **argv) {

	// Initialize ssl and thread manager

 	rtc::InitializeSSL();
 	rtc::InitRandom(rtc::Time());
 	rtc::ThreadManager::Instance()->WrapCurrentThread();
	
  	// 1. Create a PeerConnectionFactoryInterface
	CustomRunnable runnable;
	thread->Start(&runnable);

	// 2. Create a PeerConnection object with configuration and PeerConnectionObserver
 	createPeerConnection();

	// 3. ??

	// 4. Create an offer, setLocalDescription, serialize and send to remote peer
	peer_connection->CreateOffer(&create_session_description_observer, nullptr);

	rtc::CleanupSSL();

	return 0;
}
