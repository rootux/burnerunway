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
FADE_OUT_TIME = 2000 # Fading out in MS

MUSIC_PATH = '/home/pi/burnerunway/music/'
SOUND_EFFECTS_PATH = '/home/pi/burnerunway/soundeffects/'
STARTED_END_OF_COURSE_STRING = 'Started end of course animation'
ENDED_END_OF_COURSE_STRING = 'Ended end of course animation'
MOTION_DETECTED_STRING = 'Motion detected'

# If no signal was received in 30 seconds - shut down
DEFAULT_TIME_TO_SLEEP_ON_IDLE_RUNWAY = 30 
DEFAULT_TIME_TO_SLEEP_ON_IDLE_RUNWAY_END_OF_COURSE = 10 

class Burnerunway(object):
  def __init__(self):
    self.ser = None
    pygame.mixer.init()
    self.currentEffectCount = 0
    self.music = None #Sound("/home/pi/burnerunway/music/on_the_catwalk.wav")
    self.effect1 = None #Sound("/home/pi/burnerunway/music/work_it_baby.wav")
    self.channel = None
    self.allMusic = []
    self.allSoundEffects = self.getAbsoluteFiles(SOUND_EFFECTS_PATH)
    self.allMusic = self.getAbsoluteFiles(MUSIC_PATH)
    self.lastSensorSignalTime = time.time()
    self.coolDownTime = time.time()
    self.timeToSleepOnSensorIdle = DEFAULT_TIME_TO_SLEEP_ON_IDLE_RUNWAY

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

  def tryToReadSerial(self):
    try:
      serialInput = self.readLineSafe()
      if(serialInput == None):
        raise SerialException('Port not available')
      if MOTION_DETECTED_STRING in serialInput:
        self.currentEffectCount+=1
        self.lastSensorSignalTime = time.time()
        self.checkAndPlayMusic()

      elif STARTED_END_OF_COURSE_STRING in serialInput:
        self.coolDownTime = time.time()
        self.timeToSleepOnSensorIdle = DEFAULT_TIME_TO_SLEEP_ON_IDLE_RUNWAY_END_OF_COURSE
      elif ENDED_END_OF_COURSE_STRING in serialInput:
        print 'Ended'

      print serialInput
    except (SerialException, portNotOpenError) as e:
      print "Oops. probably arduino got disconnected. sleeping for 0.5 second and retrying"
      print e
      time.sleep(0.5)
      if(self.ser):
        self.ser.close()
      self.tryToConnect()

  def checkAndPlayMusic(self):
    if not (self.channel and self.channel.get_busy()):
      print "Playing music"
      self.music = random.choice(allMusic)
      self.channel = self.music.play()
    
    if(self.currentEffectCount % PLAY_EFFECT_EACH == 0):
      self.effect1 = random.choice(allSoundEffects)
      self.effect1.play()

  def fadeMusicOnIdle(self):
    if(time.time() - self.lastSensorSignalTime <= self.timeToSleepOnSensorIdle):
      return

    self.timeToSleepOnSensorIdle = DEFAULT_TIME_TO_SLEEP_ON_IDLE_RUNWAY
    self.channel.fadeout(FADE_OUT_TIME)

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
    burnerunway.tryToReadSerial()
    burnerunway.fadeMusicOnIdle()

  burnerunway.close()

if __name__ == "__main__":
  main()
