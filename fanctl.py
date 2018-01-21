#!/usr/bin/python
import paho.mqtt.client as mqtt
import sys
import serial
import signal

SERIALPORT = "/dev/ttyUSB0"
#SERIALPORT = "/tmp/ttyV0" - https://unix.stackexchange.com/questions/12359/how-can-i-monitor-serial-port-traffic
# socat /dev/ttyUSB0,raw,echo=0 SYSTEM:'tee input.txt | socat - "PTYink=/tmp/ttyV0,raw,echo=0,waitslave" | tee output.txt'
BAUDRATE = 115200
ser = serial.Serial(SERIALPORT, BAUDRATE, timeout=10)

print("MQTT Fan Remote Control")

def sigint_handler(signal, frame):
        print("Exiting")
        sys.exit(0)
signal.signal(signal.SIGINT, sigint_handler)

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("bedfan/#")
	
def on_message(client, userdata, msg):
	print(msg.topic+" "+str(msg.payload))
	if msg.topic == 'bedfan/cmd':
		if str(msg.payload) == 'ON':
			ser.write('#F3\n')
		elif str(msg.payload) == 'OFF':
			ser.write('#F0\n')
		else:
			command = '#F'+str(msg.payload)+'\n'
			ser.write(command)
	if msg.topic == 'bedfan/light':
		if str(msg.payload) == 'ON':
			ser.write("#L1\n")
		elif str(msg.payload) == 'OFF':
			ser.write("#L0\n")
		elif str(msg.payload) == 'TOGGLE':
			ser.write("#LT\n")

# Set up and connect to MQTT broker
client = mqtt.Client(client_id="BedFan")
client.username_pw_set("<username>", password="<password>")
client.on_connect = on_connect
client.on_message = on_message
client.connect("localhost", 1883, 60)
print("Running...")
client.loop_start() # Starts a background thread to handle MQTT network activity
ser.write("#HB\n") # send first heartbeat

while True:
	line = ser.readline() # blocks for 10s waiting for \n
	if line != '':
		line = line.decode('utf-8') # Convert bytes to string
		if line[0] == '#':
			if line[1] == 'L':
				if line[2] == '0':
					client.publish("bedfan/light", "OFF")
				elif line[2] == '1':
					client.publish("bedfan/light", "ON")
			elif line[1] == 'F':
				if line[2] == '0':
					client.publish("bedfan/speed", "0")
					#client.publish("bedfan/state", "OFF")
				elif line[2] == '1':
					client.publish("bedfan/speed", "1")
					#client.publish("bedfan/state", "ON")
				elif line[2] == '2':
					client.publish("bedfan/speed", "2")
					#client.publish("bedfan/state", "ON")
				elif line[2] == '3':
					client.publish("bedfan/speed", "3")
					#client.publish("bedfan/state", "ON")
				elif line[2] == '4':
					client.publish("bedfan/speed", "4")
					#client.publish("bedfan/state", "ON")
				elif line[2] == '5':
					client.publish("bedfan/speed", "5")
					#client.publish("bedfan/state", "ON")
				elif line[2] == '6':
					client.publish("bedfan/speed", "6")
					#client.publish("bedfan/state", "ON")
	ser.write("#HB\n")
