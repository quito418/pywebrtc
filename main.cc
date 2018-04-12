#include <webrtc/api/mediastreaminterface.h>
#include <webrtc/api/peerconnectioninterface.h>
#include <webrtc/base/ssladapter.h>
#include <webrtc/base/thread.h>
#include <iostream>

// Refer to the API at: https://webrtc.googlesource.com/src/+/master/api/peerconnectioninterface.h

void OnDataChannelCreated(webrtc::DataChannelInterface* channel);
void OnIceCandidate(const webrtc::IceCandidateInterface* candidate);
void OnDataChannelMessage(const webrtc::DataBuffer& buffer);
void OnAnswerCreated(webrtc::SessionDescriptionInterface* desc);

// Set up web sockets from boost

// 1. Peer Connection Factory that sets up the signaling and worker threads
rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory;
// 2. Peer Connection
rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;
// 2. Peer Connection Observer
PeerConnectionObserver peer_connection_observer(OnDataChannel, OnIceCandidate);
// 2. Configuration Settings
webrtc::PeerConnectionInterface::RTCConfiguration configuration;
// 4. Create Session Description Observer
CreateSessionDescriptionObserver create_session_description_observer(OnSuccess);
// 4. Set Session Description Observer
SetSessionDescriptionObserver set_session_description_observer;

// TODO: Functions for Peer Connection Observer
// Unsure about this??
void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) {
}

// Need JSON editor for this maybe picojson?
void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {i

}

// 4. Functions for Create Session Description Observer
void OnSuccess(webrtc::SessionDescriptionInterface* desc) { 
	peer_connection->SetLocalDescription(&set_session_description_observer, desc);
	// TODO: Figure out how to serialize (maybe stringify it or json it? then send to peer)
}

void createPeerConnectionInterface() {

    rtc::Thread *signalingThread = new rtc::Thread();
    rtc::Thread *workerThread = new rtc::Thread();

    signalingThread->SetName("signaling_thread", NULL);
    workerThread->SetName("worker_thread", NULL);

    if (!signalingThread->Start() || !workerThread->Start()) {
        return 1;
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
	createPeerConnectionInterface()

	// 2. Create a PeerConnection object with configuration and PeerConnectionObserver
    createPeerConnection();

	// 3. ??

	// 4. Create an offer, setLocalDescription, serialize and send to remote peer
	peer_connection->CreateOffer(&create_session_description_observer, nullptr);

	rtc::CleanupSSL();

	return 0;
}
