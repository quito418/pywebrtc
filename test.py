import pywebrtc 

conn = pywebrtc.Connection('hello')

conn.say_message()

print(conn.get_sdp())
