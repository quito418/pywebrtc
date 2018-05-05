import websocket
import pywebrtc
import argparse
import time

parser = argparse.ArgumentParser(description='Determine Settings')
parser.add_argument('signaling_id', metavar='i', type=int,
                   help='sets the id for the connection')

args = parser.parse_args()

# Create the object (args below)
# signaling_url: the url of the signaling server
# signaling_id: a python int that represents the ID of the session
# v4l2_device_number: the index of the fake webcam (e.g. 0 for /dev/video0)
conn = pywebrtc.Connection("wss://ccr-frontend-0.jemmons.us/ccr", args.signaling_id, 1, True)

# Wait for a client to connect on `args.signaling_id,` and perform signaling.
# Once this will block until the connection it ready to use. 
conn.wait_for_client()

# send a message
#conn.send_message('hi!')

while(True):

  # receive messages (returns python list containing strings)
  # the last one in the list is the most recently received 
  contents = conn.receive_messages()
  print("Received: ", contents)

  # we are in the process of making the `receive_messages`
  # method blocking. Sleeping for now. 
  #time.sleep(1)
  
