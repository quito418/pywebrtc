### 
#
# NOTE: you have to do the following things to get this code to work...
#
# 1. export PYTHONPATH=python/build/lib.linux-x86_64-3.5
# 2. export LD_LIBRARY_PATH=src/.libs
# 
###

import websocket
import pywebrtc
import json

try:
    import thread
except ImportError:
    import _thread as thread
import time

def on_message(ws, message):
    print("Received: " + message)

def on_error(ws, error):
    print(error)

def on_close(ws):
    print("### closed ###")

def on_open(ws):
    def run(*args):
        conn = pywebrtc.Connection("hello")
        idValues = {"type": "kind", "kind": "client", "connection_id": 1}
        print("Sending Message: " + json.dumps(idValues))
        ws.send(json.dumps(idValues))
        sdp = conn.get_sdp()
        sdpValues = {"type": "offer", "sdp":json.loads(sdp)}
        #print("--------BEGIN SDP---------")
        print(json.dumps(sdpValues))
        ws.send(json.dumps(sdpValues))
        #print("--------END SDP---------")
    thread.start_new_thread(run, ())
    


if __name__ == "__main__":
    websocket.enableTrace(True)
    ws = websocket.WebSocketApp("wss://ccr-frontend-0.jemmons.us/ccr",
                              on_message = on_message,
                              on_error = on_error,
                              on_close = on_close)
    ws.on_open = on_open
    ws.run_forever()

