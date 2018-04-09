# fakewebcam

## prerequisites

```bash
# install ffmpeg
sudo apt-get install ffmpeg

# install "video for linux" kernel module
sudo apt-get install v4l2loopback-utils

# insert the kernal module
sudo modprobe v4l2loopback devices=1
```

## run fake webcam

```bash
# send the demo video to the fake webcam
ffmpeg -i demo.mp4 -f v4l2 /dev/video0


# download a longer video
youtube-dl -f 18 https://www.youtube.com/watch?v=9bZkp7q19f0 -o longer.mp4
ffmpeg -i longer.mp4 -f v4l2 /dev/video
```
