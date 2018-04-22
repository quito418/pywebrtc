import pywebrtc 

conn = pywebrtc.Connection('hello')

print(conn.get_sdp())
