import sys
import os
import socket
import time
from pubsub import pub

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

server_ip = "127.0.0.1"
default_port = 2121
CLIENT_MESSAGE_TOPIC = "CLIENT_MESSAGE_TOPIC"
client_socket = None


def connectClient(retry: int = 1):
    while(retry > 0):
        try:
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.connect((server_ip, default_port))
            pub.sendMessage(topicName= CLIENT_MESSAGE_TOPIC,data=f"Connected to server at {server_ip}:{default_port}")
            return client_socket
        except:
            pub.sendMessage(topicName=CLIENT_MESSAGE_TOPIC, data=f"retry Connect to {server_ip}:{default_port}")
            time.sleep(2)
            retry-=1
            
    pub.sendMessage(topicName= CLIENT_MESSAGE_TOPIC,data=f"Unable Connect to {server_ip}:{default_port}")
    return None
            
def startSearching(path: str,word: str):
    global client_socket  
    
    if client_socket is None:
        client_socket = connectClient()
    
    if client_socket is not None:

        client_socket.send(path.encode("utf-8"))
        client_socket.send(word.encode("utf-8"))
    
        response = client_socket.recv(1024)
        pub.sendMessage(topicName=CLIENT_MESSAGE_TOPIC, data=response.decode("utf-8"))
        if response.lower() == "exit":
            client_socket.close()
            return

            
        
        
            
