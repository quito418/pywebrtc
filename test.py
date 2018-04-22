import pywebrtc 

conn = pywebrtc.Connection('hello')

sdp = conn.get_sdp()

print('---------- BEGIN SDP ----------')
print(sdp)
print('---------- END SDP ----------')
