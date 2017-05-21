import time
import serial
import random
from serial import SerialException, portNotOpenError
import pygame.mixer
from pygame.mixer import Sound
from os import walk
from os.path import isfile, join

ARDUINO_PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600
PLAY_EFFECT_EACH = 3

MUSIC_PATH = '/home/pi/burnerunway/music/'
SOUND_EFFECTS_PATH = '/home/pi/burnerunway/soundeffects/'

class Burnerunway(object):

  def __init__(self):
    self.ser = None
    pygame.mixer.init()
    self.currentEffectCount = 0
    self.music = None
    self.effect1 = None
    self.channel = None
    self.allSoundEffects = self.getAbsoluteFiles(SOUND_EFFECTS_PATH)
    self.allMusic = self.getAbsoluteFiles(MUSIC_PATH)
    print ('Init successfully. Effects: {} Sounds: {}'.format(len(self.allSoundEffects), len(self.allMusic)))
  
  def getAbsoluteFiles(self, path):
    filesFullPath = []
    for(root, dirs, files) in walk(path):
      for filename in files:
        filepath = join(root, filename)
        filesFullPath.append(filepath)
    return filesFullPath

  def tryToConnect(self):
    try:
      print "Trying to connect to Arduino"
      self.ser = serial.Serial(ARDUINO_PORT, BAUD_RATE)
      print "Connected!"
      return True
    except SerialException as se:
      print "Cant connect. sleeping for 2 seconds and retrying"
      time.sleep(2)
      return False

  def tryToRead(self):
    try:
      line = self.readLineSafe()
      if(line == None):
        raise SerialException('Port not available')
      if "Motion detected" in line:
        self.currentEffectCount+=1
        if not (self.channel and self.channel.get_busy()):
          print "Playing music"
 	  randomMusic = random.choice(self.allMusic)
          self.music = Sound(randomMusic)
	  print "Playing music {}".format(randomMusic)
          self.channel = self.music.play()
        
        if(self.currentEffectCount % PLAY_EFFECT_EACH == 0):
          randomSoundEffect = random.choice(self.allSoundEffects)
          print "Playing sound effect {}".format(randomSoundEffect)
          self.effect1 = Sound(randomSoundEffect)
          self.effect1.play()

      print line
    except (SerialException, portNotOpenError) as e:
      print e
      print "Oops. probably arduino got disconnected. sleeping for 0.5 second and retrying"
      print e
      time.sleep(0.5)
      if(self.ser):
        self.ser.close()
      self.tryToConnect()

  def readLineSafe(self):
    try:
       line = self.ser.readline()
       return line
    except KeyboardInterrupt:
      raise
    except:
       return None

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
