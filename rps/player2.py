import paho.mqtt.client as mqtt
import numpy as np
import time
global opp_action
opp_action = None
possible_actions = ["rock", "paper", "scissors"]

def on_connect(client, userdata, flags, rc):
  #print("Connection returned result: " + str(rc))
  client.subscribe("channel1", qos=1)

def on_disconnect(client, userdata, rc):
  if rc != 0:
    print('Unexpected Disconnect')
  else:
    print('Expected Disconnect')

def on_message(client, userdata, message):
    global opp_action
    opp_action=str(message.payload)[2:-1]

client = mqtt.Client(client_id="plyer2")

client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_message = on_message
client.connect_async('mqtt.eclipseprojects.io')
client.loop_start()

while True:

  user_action=None
  while user_action not in ["rock", "paper", "scissors"]:
      user_action=input("Enter a choice(rock, paper, scissors): ")
      user_action=user_action.lower()
  client.publish("channel2",user_action)
  while opp_action == None:
    print("Waiting for opponent's imput")
    time.sleep(2)
  #opp_action=opp_action.encode()
  print("You chose "+ str(user_action)+" , your opponent chose "+ str(opp_action))

  if user_action == opp_action:
        print(f"Both players selected {user_action}. It's a tie!")
    
  elif user_action == "rock":
        if opp_action == "scissors":
            print("Rock smashes scissors! You win!")

        else:
            print("Paper covers rock! You lose.")
     
  elif user_action == "paper":
        if opp_action == "rock":
            print("Paper covers rock! You win!")
        else:
            print("Scissors cuts paper! You lose.")
    
  elif user_action == "scissors":
    
        if opp_action == "paper":
            print("Scissors cuts paper! You win!")
        else:
            print("Rock smashes scissors! You lose.")
  user_action=None
  opp_action=None
  pass
client.loop_stop()
client.disconnect()
    