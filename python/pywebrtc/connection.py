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
    parsedMessage = json.loads(message)

    if(parsedMessage['type'] == "offer"):
        onOffer(data)
    elif(parsedMessage['type'] == "answer"):
        onAnswer(data)
    elif(parsedMessage['type'] == "candidate"):
        onCandidate(data)
    else:
        print("Undefined Message. Shutting down websocket.")
        ws.close()

def on_error(ws, error):
    print("Error: ")
    print(error)

def on_close(ws):
    print("Websocket Closed")

def create_on_open(connection):
    def on_open(ws):
        print("Websocket Open")
        def run(*args):
            # Send information about ourselves
            print("Sending Kind")
            message = json.dumps(connection.id) 
            print("Message: "+message)
            ws.send(message)
            print("Kind sent!")

            if(connection.type == "client"):
                # Get sdp information
                sdp = connection.conn.getSDP()
                print("Sending SDP")
                sdpValues = {"type": "offer", "sdp":json.loads(sdp)}
                print("Message: "+message)
                message = json.dumps(sdpValues) 
                ws.send()
                print("SDP Sent!")
        #thread.start_new_thread(run, ())
        run()
    return on_open

class Connection:

    def __init__(self, type_, id_, signalingServer):

        self.conn = pywebrtc_wrapper.PyWebRTCConnection()
        self.ws = websocket.WebSocketApp(signalingServer, 
          on_message=on_message,
          on_error=on_error,
          on_close=on_close)
        self.ws.on_open = create_on_open(self)
        self.type = type_
        self.id = {"type": "kind", "kind": type_[0], "connection_id": id_[0]}

    def run_websocket(self):
        #websocket.enableTrace(True)
        """
        self.ws.on_message = on_message
        self.ws.on_error = on_error
        self.ws.on_close = on_close
        self.ws.on_open = create_on_open(self)
        """
        
        self.ws.run_forever()



