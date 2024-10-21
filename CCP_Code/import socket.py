import socket
import json
import threading

# Configuration
UDP_IP = "10.20.30.1"  # Listen on this IP
UDP_PORT = 2000      # Port to listen on
SEND_IP = "10.20.30.101"  # IP to send messages to Bladerunner
SEND_PORT = 3001     # Port to send messages to Bladerunner

Status = ["STOPC", "STOPO", "FSLOWC", "FFASTC", "RSLOWC", "ERR", "OFLN"]

sequence_number = 0;

# Function to listen for UDP packets and handle JSON data
def listen_MCP():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Internet, UDP
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
    # Extract fields from the JSON data


    client_type = json_data.get("client_type")
    message = json_data.get("message")
    client_id = json_data.get("client_id")
    sequence_number = json_data.get("sequence_number")
    action = json_data.get("action")
    status = json_data.get("status")
    br_id = json_data.get("br_id")

    if client_id != "BR01":
        response = {
            "client_type": "CCP",
            "message": "STAT",
            "client_id": client_id,
            "sequence_number": "s_ccp",
            "status": "ERR"
        }
        send_response_toMCP(response)
        

    # intial message
    if message == "AKIN":
        send_response_toCarriage("START") # start function for BR
        print(f"Acknowledgement accepted")
    
    elif message == "STRQ":
        send_response_toCarriage("STRQ") # status request
    
    elif message == "EXEC":
        send_response_toCarriage("EXEC") #IDK

    elif message == "AKST":
        print("status acknowledgement recieved, MCP connected")

        
def handle_message_fromCarriage(data):
    #extract string

    if data == "AKIN":
        response = {
            "client_type": "CCP",
            "message": "CCIN",
            "client_id": "BR01",
            "sequence_number": sequence_number+1
        }
        send_response_toMCP(response)
        print("Successful Connection, Begin Running")
        
    else:
        response = {
            "client_type": "CCP",
            "message": "STAT",
            "client_id": "BR01",
            "sequence_number": sequence_number+1,
            "status": data
        }
        send_response_toMCP(response)
    

def send_response_toMCP(response):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # Internet, UDP
    response_json = json.dumps(response).encode('utf-8')  # Convert the response to JSON bytes
    try:
        sock.sendto(response_json, (UDP_IP, UDP_PORT))  # Send response to specified IP and port
        print(f"Sent response to MCP")
    except Exception as e:
        print(f"An error occured while sending data: {e}")

def send_response_toCarriage(data):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # Internet, UDP
    
    if isinstance(data, str):
        data = data.encode('utf-8')  # Convert string to bytes if necessary
    
    try:
        sock.sendto(data, (SEND_IP, SEND_PORT))  # Send the data to the specified IP and port
        print("Sent data to Carriage")
    except Exception as e:
        print(f"An error occurred while sending data: {e}")


if __name__ == "__main__":
    
    first = True
    
    if(first):
        handle_message_fromCarriage("AKIN")
        first = False

    
    listen_from_MCP = threading.Thread(target = listen_MCP, daemon=False)
    listen_from_Carriage = threading.Thread(target=listen_Carriage, daemon=False)

    listen_from_MCP.start()
    listen_from_Carriage.start()
    


   