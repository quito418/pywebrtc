#include <iostream>
#include <string>

#include <webrtc/api/mediastreaminterface.h>
#include <webrtc/api/peerconnectioninterface.h>
#include <webrtc/base/ssladapter.h>
#include <webrtc/base/thread.h>
#include <webrtc/base/physicalsocketserver.h>
#include "picojson/picojson.h"

class Connection {
	public:
		// Peer Connection
		rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;
		// Data Channel
		rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel;
		// SDP 
		std::string sdp_type;
		picojson::array ice_array;

	// On session success, set local description and send information to remote
	void sessionSuccess(webrtc::SessionDescriptionInterface* desc) {
		peer_connection->SetLocalDescription(&set_session_description_observer, desc);
		// TODO: Figure out how to serialize (maybe stringify it or json it? then send to peer)
	}

	// TODO: Implement this
	void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {

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

		// TODO
		void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {};
		
		// TODO
		void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface) {};

		// TODO
		void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override {};

		// TODO
		void OnRenegotiationNeeded() override {};

		// TODO
		void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override {};

		// TODO
		void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override {};

		// TODO
		void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override {};
	};

	// Used to receive callbacks from the Data Channel 
	class DataChannelObserver : public webrtc::DataChannelObserver {
		private:
			Connection &parent;
		public
			DataChannelObserver(Connection& parent) : parent(parent) {};

		// TODO
		void OnStateChange() override {};
		
		// TODO
		void OnMessage(const webrtc::DataBuffer& buffer) override {};

		// TODO
		void OnBufferedAmountChange(uint64_t previous_amount) override {};
	};

	// Used to receive callbacks from creating the session description
	class CreateSessionDescriptionObserver : public webrtc::CreateSessionDescriptionObserver {
		private:
			Connection &parent;
		public
			CreateSessionDescriptionObserver(Connection& parent) : parent(parent) {};

		// TODO
		void OnSuccess(webrtc::SessionDescriptionInterface* desc) override {};	

		// TODO
		void OnFailure(const std::string& error) override {};
	};

	// Used to receive callbacks from setting the session description (not really used but necessary as a parameter)
	class SetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver {
		private:
			Connection &parent;
		public
			SetSessionDescriptionObserver(Connection& parent) : parent(parent) {};

		//TODO
		void OnSuccess() override {};	

		// TODO
		void OnFailure(const std::string& error) override {};

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
// Refer to the API at: https://webrtc.googlesource.com/src/+/master/api/peerconnectioninterface.h

void OnDataChannel(webrtc::DataChannelInterface* channel);
void OnIceCandidate(const webrtc::IceCandidateInterface* candidate);
void OnDataChannelMessage(const webrtc::DataBuffer& buffer);
void OnAnswerCreated(webrtc::SessionDescriptionInterface* desc);
void OnSuccess(webrtc::SessionDescriptionInterface* desc);

picojson::array ice_array;

// Set up web sockets from boost

// These are all protected/abstract. Need to subclass. Use conductor.h?
// 1. Peer Connection Factory that sets up the signaling and worker threads
rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory;
// 2. Peer Connection Observer
// webrtc::PeerConnectionObserver peer_connection_observer(OnDataChannel, OnIceCandidate);
//webrtc::PeerConnectionObserver peer_connection_observer(OnIceCandidate);
// 2. Configuration Settings
webrtc::PeerConnectionInterface::RTCConfiguration configuration;
// 4. Create Session Description Observer
//webrtc::CreateSessionDescriptionObserver create_session_description_observer(OnSuccess);
// 4. Set Session Description Observer
webrtc::SetSessionDescriptionObserver set_session_description_observer;



// TODO: Functions for Peer Connection Observer
// Unsure about this??
//void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) {
//}

// Need JSON editor for this maybe picojson?
void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
  picojson::object ice;
  std::string candidate_str;
  candidate->ToString(&candidate_str);
  ice.insert(std::make_pair("candidate", picojson::value(candidate_str)));
  ice.insert(std::make_pair("sdpMid", picojson::value(candidate->sdp_mid())));
  ice.insert(std::make_pair("sdpMLineIndex", picojson::value(static_cast<double>(candidate->sdp_mline_index()))));


  /*
  Json::Value jmessage;
  Json::StyledWriter writer;
  jmessage["candidate"] = candidate_str;
  jmessage["sdpMid"] = candidate->sdp_mid();
  jmessage["sdpMLineIndex"] = candidate->sdp_mline_index();
  str = writer.write(jmessage);
  */
  
}


void createPeerConnectionInterface() {

    rtc::Thread *signalingThread = new rtc::Thread();
    rtc::Thread *workerThread = new rtc::Thread();

    signalingThread->SetName("signaling_thread", NULL);
    workerThread->SetName("worker_thread", NULL);

    if (!signalingThread->Start() || !workerThread->Start()) {
        return;
    }

    peer_connection_factory = webrtc::CreatePeerConnectionFactory(workerThread, signalingThread, NULL, NULL, NULL).release();
}

void createPeerConnection() {
	// Set up configuration for STUN server
	webrtc::PeerConnectionInterface::IceServer ice_server;
	ice_server.uri = "stun:stun.l.google.com:19302";
	configuration.servers.push_back(ice_server);

	peer_connection = peer_connection_factory->CreatePeerConnection(configuration, nullptr, nullptr, &peer_connection_observer);

	// Some people set up a data channel here (not sure why)
	// The official api says to create local media stream tracks but no reference code does (not sure why)
	
}

int main(int argc, char **argv) {

	// Initialize ssl and thread manager
  rtc::InitializeSSL();
  rtc::InitRandom(rtc::Time());
  rtc::ThreadManager::Instance()->WrapCurrentThread();
	
  // 1. Create a PeerConnectionFactoryInterface
	createPeerConnectionInterface();

	// 2. Create a PeerConnection object with configuration and PeerConnectionObserver
  createPeerConnection();

	// 3. ??

	// 4. Create an offer, setLocalDescription, serialize and send to remote peer
	peer_connection->CreateOffer(&create_session_description_observer, nullptr);

	rtc::CleanupSSL();

	return 0;
}
