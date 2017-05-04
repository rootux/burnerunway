import time
import serial

ARDUINO_PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600

def main():
  isConnected = False
  while !isConnected:
    isConnected = tryToConnect()

  while 1:
    tryToRead()

def tryToConnect():
  try:
    ser = serial.Serial(ARDUINO_PORT, BAUD_RATE)
    print "Connected!"
    return True
  except SerialException se:
    print "Cant connect. sleeping for 2 seconds and retrying"
    time.sleep(2)
    return False

def tryToRead():
  try:
      line = ser.readline()
      print line
    except SerialException se:
      print "Oops. probably arduino got disconnected"
      print se

if __name__ == "__main__":
  main()