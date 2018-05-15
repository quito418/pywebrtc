#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <sstream>

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

  std::mutex data_buffer_mutex;
  std::condition_variable data_buffer_cv;
  std::vector<std::string> data_buffer;
  
public:
    std::atomic<bool> data_channel_open;
    std::atomic<bool> video_stream_open;
    std::atomic<bool> peer_connection_failed;
    // Peer Connection
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;
    // Data Channel
    rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel;
    // SDP 
    std::string sdp_type;
    // ICE Information
    picojson::array ice_array;

    // TODO make this not as stupid...
    std::vector<std::string> getDataBuffer() {
      std::unique_lock<std::mutex> lg(data_buffer_mutex);
      
      data_buffer_cv.wait(lg, [&](){ return !data_channel_open.load() || !data_buffer.empty(); });
      
      std::vector<std::string> messages;
      for(auto message : data_buffer) {
	      messages.push_back(message);
      }
      data_buffer = std::vector<std::string>();

      return messages;
    }
  
    // On session success, set local description and send information to remote
    void sessionSuccess(webrtc::SessionDescriptionInterface* desc) {
      std::cout<< "Session success   " << sdp_type << std::endl ;
      peer_connection->SetLocalDescription(ssdo, desc);
      std::cout<< "After set local" << std::endl ;
      picojson::object information;

      std::string sdp;
      desc->ToString(&sdp);
      information.insert(std::make_pair("sdp", picojson::value(sdp)));
      information.insert(std::make_pair("type", picojson::value(sdp_type)));


      offer = picojson::value(information).serialize(true);

      std::cout<< "Pico JSON offer:    " << offer << std::endl ;
      offer_set.store(true);
      std::cout << "Calling python success callback" << std::endl;
    }

    std::string get_sdp(void) {
      while(!offer_set.load()) {  }

      offer_set.store(false);
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

    void debug(const std::string& message) {
      if(enableDebug) { 
        std::cout << message << std::endl;
      }
    }
    // Used to receive callbacks from the PeerConnection
    class PeerConnectionObs : public webrtc::PeerConnectionObserver {
      private:
        Connection &parent;

      public:
        PeerConnectionObs(Connection& parent) : parent(parent) {};

        void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override {
          parent.debug("PeerConnectionObserver Signaling Change");
        };

        void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
          std::ostringstream msg;
          msg << "PeerConnectionObserver Add Stream" << stream->label(); 
          parent.debug(msg.str());
        };

        void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
          parent.debug("PeerConnectionObserver Remove Stream");
        };

        void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override {
          std::ostringstream msg;
          msg << "PeerConnectionObserver On Data Channel(" << data_channel << ")"; 
          parent.debug(msg.str());
          parent.data_channel = data_channel;
          parent.data_channel->RegisterObserver(&parent.dco);
        };

        void OnRenegotiationNeeded() override {
          parent.debug("PeerConnectionObserver On Renegotiation Needed");
        };

        void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override {
          parent.debug("PeerConnectionObserver On Ice Connection Change");
        };

        void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override {
          parent.debug("PeerConnectionObserver On Ice Gathering Change");
        };

        void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override {
          parent.debug("PeerConnectionObserver On Ice Candidate");
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
          parent.debug("DataChannelObserver On State Change");
          webrtc::DataChannelInterface::DataState state = parent.data_channel->state();
          if (state == webrtc::DataChannelInterface::kOpen) {
            parent.debug("Data Channel is now open.");
            parent.data_channel_open.store(true);
          }

          if (state == webrtc::DataChannelInterface::kClosed) {
            parent.data_channel_open.store(false);
            parent.data_buffer_cv.notify_all();
          }
        };

        void OnMessage(const webrtc::DataBuffer& buffer) override {
          parent.debug("DataChannelObserver On Message");
          std::string buffer_contents = std::string(buffer.data.data<char>(), buffer.data.size());
          //std::cout << buffer_contents << std::endl;

	        {
	          std::unique_lock<std::mutex> lg(parent.data_buffer_mutex);
	          parent.data_buffer.push_back(buffer_contents);
	        }
	        parent.data_buffer_cv.notify_all();
        };

        void OnBufferedAmountChange(uint64_t previous_amount) override {
          std::ostringstream msg;
          msg << "DataChannelObserver On Buffered Amount Change: " << previous_amount; 
          parent.debug(msg.str());
        };
    };

    // Used to receive callbacks from creating the session description
    class CreateSessionDescriptionObs : public webrtc::CreateSessionDescriptionObserver {
      private:
        Connection &parent;
      public:
        CreateSessionDescriptionObs(Connection& parent) : parent(parent) {};

        void OnSuccess(webrtc::SessionDescriptionInterface* desc) override {
          parent.debug("CreateSessionDescriptionObserver Success Callback");
          parent.sessionSuccess(desc);
        };	

        void OnFailure(const std::string& error) override {
          std::ostringstream msg;
          msg << "CreateSessionDescriptionObserver Failure Callback. Error: " << error;
          parent.debug(msg.str());
          parent.peer_connection_failed.store(true);
        };
    };

    // Used to receive callbacks from setting the session description (not really used but necessary as a parameter)
    class SetSessionDescriptionObs : public webrtc::SetSessionDescriptionObserver {
      private:
        Connection &parent;
      public:
        SetSessionDescriptionObs(Connection& parent) : parent(parent) {};

        void OnSuccess() override {
          parent.debug("SetSessionDescriptionObserver Success Callback");
          // TODO: This is not the correct callback, video seems to be set up after python's sendCandidateInformation is finished (but nothing else is printed?)
          if(parent.data_channel_open.load()) {
            // parent.video_stream_open.store(true);
            parent.debug("************* video channel open ****************");
          }
        };	

          void OnFailure(const std::string& error) override {
            parent.debug("SetSessionDescriptionObserver Failure Callback");
            parent.peer_connection_failed.store(true);
          };

      };

    PeerConnectionObs pco;
    DataChannelObs dco;
    rtc::scoped_refptr<CreateSessionDescriptionObs> csdo;
    rtc::scoped_refptr<SetSessionDescriptionObs> ssdo;
    bool enableDebug;

    Connection(bool debug = false) :
      offer_set(false),
      data_channel_open(false),
      video_stream_open(false),
      peer_connection_failed(false),
      pco(*this),
      dco(*this),
      csdo(new rtc::RefCountedObject<CreateSessionDescriptionObs>(*this)),
      ssdo(new rtc::RefCountedObject<SetSessionDescriptionObs>(*this)) {
        enableDebug = debug;
      }
};
