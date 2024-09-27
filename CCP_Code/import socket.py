import socket
import json
import time 

start_time = time.monotonic()
elapsed_time = time.monotonic() - start_time

# Configuration
UDP_IP = "10.20.30.1"  # Listen on all interfaces
UDP_PORT = 2001      # Port to listen on
SEND_IP = "10.20.30.101"  # IP to send messages to
SEND_PORT = 3001     # Port to send messages to

all_Clear = False

BR_State = ""

current_station_id = ""
# Function to listen for UDP packets and handle JSON data
def listen_udp():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # Internet, UDP
    sock.bind((UDP_IP, UDP_PORT))  # Bind to specified IP and port
    print(f"Listening for UDP packets on {UDP_IP}:{UDP_PORT}...")
    
    while True:
        data, addr = sock.recvfrom(1024)  # Buffer size is 1024 bytes
        json_str = data.decode('utf-8')  # Decode the received bytes to a string
        print(elapsed_time) # print time
        try:
            json_data = json.loads(json_str)  # Parse the JSON string
            print(f"Received JSON from {addr}: {json_data}")  # Print the parsed JSON
            handle_message(json_data)  # Process the message
        except json.JSONDecodeError:
            print(f"Received invalid JSON from {addr}: {json_str}")

def handle_message(json_data):
    # Extract fields from the JSON data
    client_type = json_data.get("client_type")
    message = json_data.get("message")
    client_id = json_data.get("client_id")
    timestamp = json_data.get("timestamp")
    action = json_data.get("action")
    status = json_data.get("status")
    station_id = json_data.get("station_id")

    # intial message
    if message == "AKIN":
        response = {
            "client_type": "ccp",
            "message": "CCIN",
            "client_id": "BR01",
            "timestamp": str(elapsed_time)
        }
    if message == "STAT":
        response = {
            "client_type": "ccp",
            "message": "STAT",
            "client_id": "BR01",
            "timestamp": str(elapsed_time),
            "status" : BR_State,
            "station_id": current_station_id
            }
        
    

    send_response(response)

def send_response(response):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # Internet, UDP
    response_json = json.dumps(response).encode('utf-8')  # Convert the response to JSON bytes
    sock.sendto(response_json, (SEND_IP, SEND_PORT))  # Send response to specified IP and port
    print(f"Sent response to {SEND_IP}:{SEND_PORT}: {response}")

if __name__ == "__main__":
    listen_udp()  # Call the function to start listening