from socket import *

serverName = 'hostname'
serverPort = 12000
clientSocket = socket(AF_INET, SOCK_DGRAM) #UDP socket

message = input("Input lowercase letters")
clientSocket.sendto(message.encode(), (serverName, serverPort)) #attatch server name, port  to message; send into socket


modifiedMessage, serverAddress = clientSocket.recvfrom(2048) #read reply characters from socket into string
print(modifiedMessage.decode()) #print out recieved string and close socket
clientSocket.close()
