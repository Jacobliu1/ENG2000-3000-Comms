import socket
import json
import threading

# Configuration
UDP_IP = "10.20.30.1"  # Listen on this IP
UDP_PORT = 2000      # Port to listen on
SEND_IP = "10.20.30.101"  # IP to send messages to Bladerunner
SEND_PORT = 3001     # Port to send messages to Bladerunner

Status = ["STOPC, STOPO, FSLOWC, FFASTC, RSLOWC,ERR,OFLN"]

sequenceCounting = 1

# Function to listen for UDP packets and handle JSON data
def listen_MCP():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # Internet, UDP
    sock.bind((UDP_IP, UDP_PORT))  # Bind to specified IP and port
    print("Listening for MCP packets")
    
    while True:
        data, addr = sock.recvfrom(1024)  # Buffer size is 1024 bytes
        json_str = data.decode('utf-8')  # Decode the received bytes to a string
   
        try:
            json_data = json.loads(json_str)  # Parse the JSON string
            print(f"Received JSON from {addr}: {json_data}")  # Print the parsed JSON
            handle_message_fromMCP(json_data)  # Process the message
        except json.JSONDecodeError:
            print(f"Received invalid JSON from {addr}: {json_str}")

def listen_Carriage():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((SEND_IP, SEND_PORT))
    print("Listening for UDP .data files from BladeRunner")

    while True:
        data, addr = sock.recvfrom(1024)  # Buffer size is 1024 bytes
        try:
            string_data = data.decode('utf-8')  # Decode the received bytes to a string
            print(f"Received string/data from {addr}: {string_data}")  # Print the received string
            handle_message_fromCarriage(string_data)  # Process the string message
        except UnicodeDecodeError:
            print(f"Received invalid UTF-8 data from {addr}: {data}")

def handle_message_fromMCP(json_data):
    # Extract fields from the JSON data\

    statusToSend = ""

    client_type = json_data.get("client_type")
    message = json_data.get("message")
    client_id = json_data.get("client_id")
    sequence_number = json_data.get("sequence_number")

    if client_id != "BR01":
        statusToSend = Status[5]
    

    # intial message
    if message == "AKIN":
        send_response_toCarriage("AKIN")
    
    elif message == "STAT":
        response = {
            "client_type": "ccp",
            "message": "STAT",
            "client_id": "BR01",
            "sequence_number":sequence_number+1 ,
            "status" : statusToSend
            }
        send_response_toMCP(response)
    #other responses
        
def handle_message_fromCarriage(data):
    #extract string

    if data == "AKIN":
        response = {
            "client_type": "CCP",
            "message": "CCIN",
            "client_id": "BR01",
            "sequence_number": 0
        }
        send_response_toMCP(response)
    #Other functions...
    
    
def send_response_toMCP(response):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # Internet, UDP
    response_json = json.dumps(response).encode('utf-8')  # Convert the response to JSON bytes
    sock.sendto(response_json, (UDP_IP, UDP_PORT))  # Send response to specified IP and port
    print(f"Sent response to {UDP_IP}:{UDP_PORT}: {response}")

def send_response_toCarriage(data):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # Internet, UDP
    
    if isinstance(data, str):
        data = data.encode('utf-8')  # Convert string to bytes if necessary
    
    try:
        sock.sendto(data, (SEND_IP, SEND_PORT))  # Send the data to the specified IP and port
        print(f"Sent data to {SEND_IP}:{SEND_PORT}")
    except Exception as e:
        print(f"An error occurred while sending data: {e}")


if __name__ == "__main__":
    listen_from_MCP = threading.Thread(target=listen_MCP, daemon=True)
    listen_from_Carriage = threading.Thread(target=listen_Carriage, daemon=True)

    listen_from_MCP.start()
    listen_from_Carriage.start()
    


   