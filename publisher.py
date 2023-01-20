import paho.mqtt.client as mqtt
import numpy as np

def on_connect(client, userdata, flags, rc):
    print("Connection returned result: " + str(rc))
   # client.subscribe("ece180d/test", qos=1)

def on_disconnect(client, userdata, rc):
    if rc !=0:
        print('Unexpected Disconnect')
    else:
        print('Expected Disconnect')

def on_message(client, userdata, message):
    print('Received message: "' + str(message.payload) + '" on topic "' +
message.topic + '" with QoS ' + str(message.qos))
# 1. create a client instance.
client = mqtt.Client()
client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_message = on_message
client.connect_async('mqtt.eclipseprojects.io')
client.loop_start()
print('Publishing...')
for i in range(10):
    client.publish("ece180d/test", float(np.random.random(1)),qos=1)
client.loop_stop()
client.disconnect()