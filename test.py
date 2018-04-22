### 
#
# NOTE: you have to do the following things to get this code to work...
#
# 1. export PYTHONPATH=python/build/lib.linux-x86_64-3.5
# 2. export LD_LIBRARY_PATH=src/.libs
# 
###

import pywebrtc 

conn = pywebrtc.Connection('hello')

sdp = conn.get_sdp()

print('---------- BEGIN SDP ----------')
print(sdp)
print('---------- END SDP ----------')
