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

parser = argparse.ArgumentParser(description='Determine Settings')
parser.add_argument('id', metavar='i', type=int, nargs='+',
                   help='sets the id for the connection')
parser.add_argument('type', metavar='t', type=str, nargs='+',
                   help='sets the type of the connection')

args = parser.parse_args()
<<<<<<< HEAD

=======
>>>>>>> fa00bdd98edcd7f3b0024a9fe18b10c5ddb09a68
conn = pywebrtc.Connection(args.type[0], args.id[0], "wss://ccr-frontend-0.jemmons.us/ccr")
conn.run_websocket()
