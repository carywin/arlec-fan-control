# arlec-fan-control
Arduino and Python code for controlling an Arlec ceiling fan/light via MQTT.

Depends on Paho-MQTT eg. pip install paho-mqtt

The Arduino is connected to the host machine via USB serial port. It has simple 433 MHz transmitter and receiver modules connected to it. It listens for RF commands from the Arlec remote and sends RF commands to the ceiling fan and light combo.

The python script connects to the Arduino's serial port and relays commands to/from the MQTT broker.
