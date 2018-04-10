#include <webrtc/api/mediastreaminterface.h>
#include <webrtc/api/peerconnectioninterface.h>
#include <webrtc/base/ssladapter.h>
#include <webrtc/base/thread.h>


int main(int argc, char **argv) {
    rtc::InitializeSSL();
    rtc::InitRandom(rtc::Time());
    rtc::ThreadManager::Instance()->WrapCurrentThread();
    
    rtc::Thread *signalingThread = new rtc::Thread();
    rtc::Thread *workerThread = new rtc::Thread();

    signalingThread->SetName("signaling_thread", NULL);
    workerThread->SetName("worker_thread", NULL);

    if (!signalingThread->Start() || !workerThread->Start()) {
        return 1;
    }

    // questionable arguments compared to sample
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pcfactory = 
        webrtc::CreatePeerConnectionFactory(workerThread, signalingThread,
                                            NULL, NULL, NULL);
}
