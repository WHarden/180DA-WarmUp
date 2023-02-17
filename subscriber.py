import paho.mqtt.client as mqtt
import time
import numpy as np

# 0. define callbacks - functions that run when events happen.
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connection returned result: " + str(rc))
    client.subscribe("ece180d/test", qos=1)
    client.publish("Messagae receieved")

# Subscribing in on_connect() means that if we lose the connection and
# reconnect then subscriptions will be renewed.
# The callback of the client when it disconnects.
def on_disconnect(client, userdata, rc):
    if rc !=0:
        print('Unexpected Disconnect')
    else:
        print('Expected Disconnect')

# The default message callback.
# (you can create separate callbacks per subscribed topic)
def on_message(client, userdata, message):

    print('Received message: "' + str(message.payload) + '" on topic "' +
message.topic + '" with QoS ' + str(message.qos))
    #client.publish("Messagae receieved")

# 1. create a client instance.
client = mqtt.Client()


# add additional client options (security, certifications, etc.)
# many default options should be good to start off.
# add callbacks to client.
client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_message = on_message
# 2. connect to a broker using one of the connect*() functions.
# client.connect_async("test.mosquitto.org")
client.connect_async('mqtt.eclipseprojects.io')
# client.connect("test.mosquitto.org", 1883, 60)
# client.connect("mqtt.eclipse.org")
# 3. call one of the loop*() functions to maintain network traffic flow with the broker.
client.loop_start()
print('Publishing...')
for i in range(10):
    client.publish("ece180d/test", float(np.random.random(1)),qos=2)
# client.loop_forever()
while True: 
    #client.publish("ece180d/test",float(np.random.random(1)))
    time.sleep(1)
    pass

    # perhaps add a stopping condition using some break or something.
 # do your non-blocked other stuff here, like receive IMU data or something.
# use subscribe() to subscribe to a topic and receive messages.
# use publish() to publish messages to the broker.
# use disconnect() to disconnect from the broker.
client.loop_stop()
client.disconnect()