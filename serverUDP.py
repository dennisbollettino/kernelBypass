import socket

# Simple python udp server
IP = "0.0.0.0" 
PORT = 8000
BUFFER_SIZE = 32  

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((IP, PORT))

print(f"UDP server listening on {IP}:{PORT}")

while True:
    data, addr = sock.recvfrom(BUFFER_SIZE)

