import time
import serial
from serial import SerialException 

ARDUINO_PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600

def main():
  ser = None
  isConnected = False
  while not isConnected:
    isConnected = tryToConnect(ser)

  while 1:
    tryToRead(ser)

def tryToConnect(ser):
  try:
    ser = serial.Serial(ARDUINO_PORT, BAUD_RATE)
    print "Connected!"
    return True
  except SerialException as se:
    print "Cant connect. sleeping for 2 seconds and retrying"
    time.sleep(2)
    return False

def tryToRead(ser):
  try:
    line = ser.readline()
    print line
  except SerialException as se:
    print "Oops. probably arduino got disconnected"
    print se

if __name__ == "__main__":
  main()