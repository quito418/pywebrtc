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
parser.add_argument('id', metavar='i', type=int, nargs='+',
                   help='sets the id for the connection')
parser.add_argument('type', metavar='t', type=str, nargs='+',
                   help='sets the type of the connection')

args = parser.parse_args()
conn = pywebrtc.Connection(args.type[0], args.id[0], "wss://ccr-frontend-0.jemmons.us/ccr")
conn.run_websocket()
conn.send_string('hi!')
contents = conn.read_from_data_channel()
while(True):
  contents = conn.read_from_data_channel()
  print("Received: ", contents)
  msg = input("Send message through data channel: ")
  conn.send_string(msg)
