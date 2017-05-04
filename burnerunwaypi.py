import serial
ser = serial.Serial('/dev/ttyUSB0', 9600)

def main():
  while 1:
    ser.readline()

if __name__ == "__main__":
  main()