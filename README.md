# Burnerunway
The Burnerunway Midburn art project

Code for Arduino and Pi.

More about this creation can be found here: https://dreams.midburn.org/dreams/87?lang=en

## Raspberry -> Arduino
We're using https://github.com/pyserial/pyserial to communicate between the Pi and the Arduino

## Pi
The pi code know to recover in case the arduino got disconnect and connected so the app should have a 100% Uptime
If no signal is received in 30 seconds - the music shuts off

### Pi setup
The pi should run RASPBIAN JESSIE WITH PIXEL so that the sound library pygame would be already installed
Setting the local to en-us
Allow ssh

On the Pi
```
sudo apt-get install python-pip
sudo pip install pyserial
sudo pip install pygame
git clone https://github.com/rootux/burnerunway
sudo raspi-config
Advanced-> Audio-> Force Jack
```

## Pi start on start of pi
```
echo "python /home/pi/burnerunway/burnerunwaypi.py" | sudo tee --append /etc/init.d/superscript
sudo chmod 755 /etc/init.d/superscript
sudo update-rc.d superscript defaults

sudo nano ~/.config/lxsession/LXDE-pi/autostart
# Add this as last line
@python /home/pi/burnerunway/burnerunwaypi.py
```