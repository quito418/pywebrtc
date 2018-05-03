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
import argparse
import time

parser = argparse.ArgumentParser(description='Determine Settings')
parser.add_argument('signaling_id', metavar='i', type=int,
                   help='sets the id for the connection')

#parser.add_argument('type', metavar='t', type=str,
#                   help='sets the type of the connection')

args = parser.parse_args()
#conn = pywebrtc.Connection(args.type[0], args.id[0], "wss://ccr-frontend-0.jemmons.us/ccr")
conn = pywebrtc.Connection("wss://ccr-frontend-0.jemmons.us/ccr", args.signaling_id, 1)
#conn.run_websocket()
conn.wait_for_client()

conn.send_message('hi!')
while(True):
  contents = conn.receive_messages()
  
  print("Received: ", contents)
  time.sleep(1)

  #msg = input("Send message through data channel: ")
  #conn.send_string(msg)
