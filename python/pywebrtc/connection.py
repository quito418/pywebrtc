import pywebrtc._ext.pywebrtc as pywebrtc_wrapper
import json
import threading
import time
import websocket

import logging; logging.basicConfig(level=logging.INFO)

class Connection:

    
    def __init__(self, signaling_url, signaling_id, video_device_path):
        self.logger = logging.getLogger('signaling_id:{}_video_device_path:{}'.format(signaling_id, video_device_path))
        
        self.signaling_url = signaling_url
        self.signaling_id = signaling_id
        self.signaling_kind = 'server'
        self.signaling_thread = threading.Thread(target=self._signaling_handler)
        
        self.video_device_path = video_device_path

        self.rtc_connection = pywebrtc_wrapper.PyWebRTCConnection()
        self.ws = websocket.WebSocketApp(self.signaling_url, 
                                         on_message=self._on_message,
                                         on_error=self._on_error,
                                         on_close=self._on_close,
                                         on_open = self._on_open)

        self.id = {"type": "kind", "kind": self.signaling_kind, "connection_id": self.signaling_id}


    def wait_for_client(self):
        self.ws.run_forever()
        self.signaling_thread.join()

        
    def send_message(self, message):
        self.rtc_connection.sendString(message)

        
    def receive_messages(self):
        return self.rtc_connection.readFromDataChannel()

    
    def _on_error(self, ws, error):
        self.logger.error("an error occured on the websocket connection to the signaliing server: " + error)

        
    def _on_close(self, ws):
        self.logger.info("websocket closed")

        
    def _on_open(self, ws):
        self.logger.info("websocket open")
        self.signaling_thread.start()

        
    def _signaling_handler(self):

        # Send information about ourselves
        self.logger.info("Sending Kind")
        message = json.dumps(self.id) 
        self.ws.send(message)
        
        self.logger.info("kind sent! waiting for client to connect.")
        
        # wait until data channel is open
        while(not self.rtc_connection.datachannelOpen()):
            time.sleep(0.1)
            
        # add video/audio streams
        self.rtc_connection.addTracks(0)
        sdp = self.rtc_connection.getSDP()
        
        self.logger.info("Sending SDP")
        sdpValues = {"type": "offer", "sdp": json.loads(sdp)}            
        message = json.dumps(sdpValues)
        self.ws.send(message)
        
        self.logger.info("SDP Sent!")
        
        # wait until video and audio are ready?
        time.sleep(5) # for now, just wait 5 seconds
        self.ws.close()
        
        
    def _on_message(self, ws, data):
        self.logger.info("Received: " + data)
        parsedData = json.loads(data)

        if(parsedData['type'] == "offer"):
            answer = self._on_rtc_offer(parsedData['sdp']['sdp'])
            sdpValues = {"type": "answer", "sdp": json.loads(answer)}
            message = json.dumps(sdpValues)
            self.ws.send(message)
            self._send_candidate_information()

        elif(parsedData['type'] == "answer"):
            self._on_rtc_answer(parsedData['sdp']['sdp'])
            self._send_candidate_information()

        elif(parsedData['type'] == "candidate"):
            candidate = parsedData['candidate']
            self._on_rtc_andidate(json.dumps([candidate]))

        else:
            error_message = "Undefined message received on from signaling server. Shutting down websocket."
            self.logger.error(error_message)
            raise Exception(error_message)

            
    def _on_rtc_offer(self, offer):
        self.logger.info("received an offer: " + offer)
        return self.rtc_connection.receiveOffer(offer)

    
    def _on_rtc_answer(self, answer):
        self.logger.info("received an answer: " + answer)
        self.rtc_connection.receiveAnswer(answer)

        
    def _on_rtc_candidate(self, candidate):
        self.logger.info("received a candidate: " + candidate)
        self.rtc_connection.setICEInformation(candidate) 

        
    def _send_candidate_information(self):
        self.logger.info("sending candidate information")

        jsonICE = json.loads(self.rtc_connection.getICEInformation())
        for iceCandidate in jsonICE:
            candidateValue = {"type": "candidate", "candidate": iceCandidate}
            candidateMessage = json.dumps(candidateValue)
            self.ws.send(candidateMessage)
            self.logger.info("Message: " + candidateMessage)

        self.logger.info("done! sending candidate information")


