

def connectMCP (response):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # Internet, UDP
    response_json = json.dumps(response).encode('utf-8')  # Convert the response to JSON bytes
    sock.sendto(response_json, (UDP_IP, UDP_PORT))  # Send response to specified IP and port
    print(f"Sent response to {UDP_IP}:{UDP_PORT}: {response}")