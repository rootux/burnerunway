import time
import serial
from serial import SerialException 
import pyglet

ARDUINO_PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600

class Burnerunway(object):

  def __init__(self):
    self.ser = None
    self.music = source = pyglet.media.load("music/on_the_catwalk.wav")
    self.effect1 = pyglet.media.load("music/work_it_baby.wav")

  def tryToConnect(self):
    try:
      self.ser = serial.Serial(ARDUINO_PORT, BAUD_RATE)
      print "Connected!"
      return True
    except SerialException as se:
      print "Cant connect. sleeping for 2 seconds and retrying"
      time.sleep(2)
      return False

  def tryToRead(self):
    try:
      line = self.ser.readline()
      if "Motion detected" in line:
        print "Playing music"
        self.music.play()

      print line
    except SerialException as se:
      print "Oops. probably arduino got disconnected. sleeping for 0.5 second and retrying"
      print se
      time.sleep(0.5)
      if(self.ser):
        self.ser.close()
      self.tryToConnect()

  def close(self):
      if(self.ser):
        self.ser.close()

def main():
  isConnected = False
  burnerunway = Burnerunway() 
  while not isConnected:
    isConnected = burnerunway.tryToConnect()

  while 1:
    burnerunway.tryToRead()

  burnerunway.close()

if __name__ == "__main__":
  main()