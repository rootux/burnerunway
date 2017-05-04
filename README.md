# Burnerunway
The Burnerunway Midburn art project

Code for Arduino and Pi.

More about this creation can be found here: https://dreams.midburn.org/dreams/87?lang=en

## Raspberry -> Arduino
We're using https://github.com/pyserial/pyserial to communicate between the pi and the arduino

## Pi
The pi code know to recover in case the arduino got disconnect and connected so the app should have a 100% Uptime

On the Pi
```
sudo apt-get install python-pip
sudo pip install pyserial
sudo pip install daemonize
```