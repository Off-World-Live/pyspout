import cv2
from PySpout import SpoutSender
from OpenGL.GL import * 
import signal
import sys


vid = cv2.VideoCapture(1)

width = 640
height = 480

if vid.isOpened():
    width = int(vid.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(vid.get(cv2.CAP_PROP_FRAME_HEIGHT))


sender = SpoutSender("MyName", width,height, GL_RGB)

# Capture exit to clear everything up
def signal_handler(sig, frame):
    print("Exiting")
    sender.release()
    vid.release()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

print("starting spout sender....")

while True:
    # get one frame
    ret, frame = vid.read()
    frame = cv2.flip(frame, 1)
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    if not sender.send_image(frame, False):
        print("Send failed. Exiting")
        break

print("Exiting due to failure")

sender.release()
vid.release()
