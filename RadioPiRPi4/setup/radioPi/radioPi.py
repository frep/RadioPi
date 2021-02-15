#!/usr/bin/python

import smbus
import RPi.GPIO as GPIO
import subprocess
import logging
from time import sleep
from mpd import MPDClient

# powerPin to prevent circuit to turn off
powerOnPin = 16
# arduino i2c slave address
address = 0x04
bus = smbus.SMBus(1)    # 0 = /dev/i2c-0 (port I2C0), 1 = /dev/i2c-1 (port I2C1)
# MPD settings to get current states
client = MPDClient()
# logging settings
#   filemode="w" : overwrites file
#   level=logging.INFO : No DEBUG, but INFO, WARNING and ERROR are logged
logger = logging.getLogger(__name__)
hdlr = logging.FileHandler('/var/tmp/radioPi.log', mode='w')
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
hdlr.setFormatter(formatter)
logger.addHandler(hdlr) 
logger.setLevel(logging.INFO)
# radio state
radioIsOn = False

def setup():
 #GPIO.cleanup()
 GPIO.setmode(GPIO.BCM)
 GPIO.setup(powerOnPin, GPIO.OUT)
 GPIO.output(powerOnPin, GPIO.HIGH)
 logger.debug("PowerPin set high")
 # connect to MPD
 try:
  client.connect("localhost", 6600)
  logger.debug("connected to MPDClient")
 except Exception, e:
  logger.error('Failed to connect to MPDClient', exc_info=True)
 # assert mixer settings
 try:
  subprocess.call(['amixer', 'sset', 'Master', '75%'])
  logger.debug("amixer Master level set")
 except Exception, e:
  logger.error('Failed to set amixer Master', exc_info=True)
 logger.info("App started (setup done)")
 checkRadioState()
 # set default volume
 changeVolume('8')

def readData():
 try:
  number = bus.read_byte(address)
 except IOError, err:
  return -1
 return number

def checkRadioState():
 global radioIsOn
 try:
  status = client.status()
  if status['state'] == 'stop':
   logger.debug("read state stop")
   radioIsOn = False
  if status['state'] == 'play':
   logger.debug("read state play")
   radioIsOn = True
 except Exception, e:
  logger.error('Failed to get mpd state', exc_info=True)
 logger.debug("checkRadioState done")

def shortPress():
 global radioIsOn
 logger.debug("short press detected")
 # check, if player is currently running
 checkRadioState()
 if radioIsOn:
  logger.info("player is playing. Stop it!")
  try:
   #subprocess.call(['mpc', 'stop'])
   logger.info(subprocess.check_output(['mpc', 'stop']))
   radioIsOn = False
  except Exception, e:
   logger.error('Failed to call: mpc stop', exc_info=True)
 else:
  logger.info("player is stopped. Start it!")
  try:
   #subprocess.call(['mpc', 'play'])
   logger.info(subprocess.check_output(['mpc', 'play']))
   radioIsOn = True
  except Exception, e:
   logger.error('Failed to call: mpc play', exc_info=True)

def longPress():
 logger.debug("long press detected")
 # poweroff RPi
 subprocess.call(['poweroff'])

def changeVolume(value):
 # change current volume by value
 logger.debug("change volume by %s" %value)
 #subprocess.call(['mpc', 'volume', str(value)])
 logger.info(subprocess.check_output(['mpc', 'volume', str(value)]))

def processData():
 if data == -1:
  # data invalid  
  return
 logger.debug("data is: 0x%02x" %data)
 # decode key presses
 keyPress = (data & 3)
 if keyPress == 1:
  shortPress()
 elif keyPress == 2:
  longPress()
 # decode volume changes
 volumeChange = data>>4
 if volumeChange > 0:
  # check if volume shoud go up or down
  if (data & 8)>0:
   # decrease volume
   changeVolume('-'+str(volumeChange))
  else:
   # increase volume
   changeVolume('+'+str(volumeChange))

if __name__ == '__main__':
 # setup
 setup()
 # loop
 try:
  while True:
   sleep(0.5)
   data = readData()
   processData()
   client.ping()
 # Stop on Ctrl+C and clean up
 except KeyboardInterrupt:
  GPIO.output(powerOnPin, GPIO.LOW)
  GPIO.cleanup()
  client.close()
  client.disconnect()
  logger.info("script terminated by KeyboardInterrupt")