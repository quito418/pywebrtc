#pragma once

#include <atomic>
#include <vector>

#include <webrtc/api/mediastreaminterface.h>
#include <webrtc/api/peerconnectioninterface.h>
#include <webrtc/base/ssladapter.h>
#include <webrtc/base/thread.h>
#include <webrtc/base/flags.h>
#include <webrtc/base/physicalsocketserver.h>
#include <webrtc/api/audio_codecs/builtin_audio_decoder_factory.h>
#include <webrtc/api/audio_codecs/builtin_audio_encoder_factory.h>

#include "picojson.h"

#define WEBRTC_LINUX 1
#define WEBRTC_POSIX 1

class CustomRunnable : public rtc::Runnable {
private:
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>  &peer_connection_factory_;
  std::mutex &peer_connection_factory_mutex_; 
public:
  
  CustomRunnable(rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> &peer_connection_factory,
		 std::mutex &peer_connection_factory_mutex) :
    peer_connection_factory_(peer_connection_factory),
    peer_connection_factory_mutex_(peer_connection_factory_mutex)
  {}
    
  void Run(rtc::Thread* subthread) override {
      peer_connection_factory_ = webrtc::CreatePeerConnectionFactory(webrtc::CreateBuiltinAudioEncoderFactory(), webrtc::CreateBuiltinAudioDecoderFactory());
      peer_connection_factory_mutex_.unlock();
      if (peer_connection_factory_.get() == nullptr) {
        std::cout << "Error on CreatePeerConnectionFactory." << std::endl;
        return;
      }

      subthread->Run();
    }
};

class Connection {
private:
  std::string offer;
  std::atomic<bool> offer_set;
  
public:
    std::atomic<bool> datachannel_open;
    // Peer Connection
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;
    // Data Channel
    rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel;
    // SDP 
    std::string sdp_type;
    // ICE Information
    picojson::array ice_array;
    std::vector<std::string> data_buffer;

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


      offer = picojson::value(information).serialize(true);
      offer_set.store(true);
      std::cout << "Calling python success callback" << std::endl;
    }

    std::string get_sdp(void) {
      while(!offer_set.load()) {  }

      return offer;    
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
      std::cout << "ICE Candidate added" << std::endl;
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
          std::cout << "PeerConnectionObserver Add Stream" << stream->label() << std::endl;
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
          parent.OnIceCandidate(candidate);
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
          webrtc::DataChannelInterface::DataState state = parent.data_channel->state();
          if (state == webrtc::DataChannelInterface::kOpen) {
            std::cout << "Data Channel is now open." << std::endl;
            parent.datachannel_open.store(true);
            std::cout << "Datachannel_open: " << parent.datachannel_open.load() << std::endl;
          }
        };

        void OnMessage(const webrtc::DataBuffer& buffer) override {
          std::cout << "DataChannelObserver On Message" << std::endl;
          std::string buffer_contents = std::string(buffer.data.data<char>(), buffer.data.size());
          std::cout << buffer_contents << std::endl;
          parent.data_buffer.push_back(buffer_contents);
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
      offer_set(false),
      datachannel_open(false),
      pco(*this),
      dco(*this),
      csdo(new rtc::RefCountedObject<CreateSessionDescriptionObs>(*this)),
      ssdo(new rtc::RefCountedObject<SetSessionDescriptionObs>(*this)) {}
};
