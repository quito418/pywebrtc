import pywebrtc._ext.pywebrtc as pywebrtc_wrapper
import websocket
import json
try:
    import thread
except ImportError:
    import _thread as thread
import time

def onOffer(offer):
    print("Offer: " + offer)

def onAnswer(answer):
    print("Answer: " + answer)

def onCandidate(candidate):
    print ("Candidate: " + candidate)

def on_message(ws, message):
    print("Received: " + message)
    var parsedMessage = json.loads(message)

    if(parsedMessage.type == "offer"):
        onOffer(data)
    elif(parsedMessage.type == "answer"):
        onAnswer(data)
    elif(parsedMessage.type == "candidate"):
        onCandidate(data)
    else:
        print("Undefined Message. Shutting down websocket.")
        ws.close()

def on_error(ws, error):
    print(error)

def on_close(self, ws):
    print("Websocket Closed")

def on_open(self, ws):
    def run(*args):
        # Send information about ourselves
        ws.send(json.dumps(self.id))

        if(self.type == "client"):
            # Get sdp information
            sdp = self.conn.getSDP()
            sdpValues = {"type": "offer", "sdp":json.loads(sdp)}
            ws.send(json.dumps(sdpValues))
    thread.start_new_thread(run, ())

class Connection:

    def __init__(self, type_, id_, signalingServer):

        self.conn = pywebrtc_wrapper.PyWebRTCConnection()
        self.ws = websocket.WebSocketApp(signalingServer)
        self.type = type_
        self.id = {"type": "kind", "kind": type_, "connection_id": id_}

    def run_websocket(self):
        websocket.enableTrace(True)
        self.ws.on_message = on_message
        self.ws.on_error = on_error
        self.ws.on_close = on_close
        self.ws.on_open = on_open
        
        self.ws.run_forever()



