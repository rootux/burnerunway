import time
import serial
from serial import SerialException 

ARDUINO_PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600

class Burnerunway(object):

  def __init__(self):
    self.ser = None

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
      print line
    except SerialException as se:
      print "Oops. probably arduino got disconnected. sleeping for 0.5 second and retrying"
      print se
      time.sleep(0.5)
      self.tryToConnect()

def main():
  isConnected = False
  burnerunway = Burnerunway() 
  while not isConnected:
    isConnected = burnerunway.tryToConnect()

  while 1:
    burnerunway.tryToRead()

if __name__ == "__main__":
  main()