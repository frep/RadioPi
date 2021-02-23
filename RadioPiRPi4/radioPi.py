#!/usr/bin/python

import paho.mqtt.client as mqtt
import subprocess
import json
#from time import sleep


MQTT_SERVER = "localhost"
MQTT_PORT   = 1883
MQTT_PATH   = "volumio"

client = mqtt.Client()


def setup():
 client.on_connect = on_connect
 client.on_message = on_message
 client.will_set('raspberry/status', b'{"status": "Off"}')
 client.connect(MQTT_SERVER, MQTT_PORT, 60)

def on_connect(client, userdata, flags, rc):
 print("Connected to "+MQTT_SERVER+":"+str(MQTT_PORT)+" with result code "+str(rc))
 client.subscribe(MQTT_PATH)

def on_message(client, userdata, msg):
 #print(msg.topic+" "+str(msg.payload))
 command = str(msg.payload)
 if command == "toggle":
  subprocess.check_output(['volumio', 'toggle'])
 elif command == "status":
  dict_obj = json.loads(subprocess.check_output(['volumio', 'status']))
  print("Status: "+dict_obj.get('status'))
 else:
  print(msg.topic+" "+str(msg.payload))


if __name__ == '__main__':

 # setup
 setup()

 # loop
 try:
  #client.loop_forever()
  while True:
   client.loop(0.1)
 # Stop on Ctrl+C and clean up
 except KeyboardInterrupt:
  print("script terminated by KeyboardInterrupt")
