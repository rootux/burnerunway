import time
import serial
from serial import SerialException 
import pygame.mixer
from pygame.mixer import Sound

ARDUINO_PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600
PLAY_EFFECT_EACH = 3

class Burnerunway(object):

  def __init__(self):
    self.ser = None
    pygame.mixer.init()
    self.currentEffectCount = 0
    self.music = Sound("music/on_the_catwalk.wav")
    self.effect1 = Sound("music/work_it_baby.wav")
    self.isPlaying = False

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
        self.currentEffectCount+=1
        if not self.isPlaying:
	  self.isPlaying = True
          print "Playing music"
          self.music.play()
        
	if(self.currentEffectCount % PLAY_EFFECT_EACH == 0):
          self.effect1.play()

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
