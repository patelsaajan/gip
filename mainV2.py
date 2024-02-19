import socket
import time
from aStarCode import *

Aip = '192.168.0.101'
laptopIP = '192.168.0.100'

def createCommands(coordinates): 
    orderLists = []
    for i in range(len(coordinates)):
        if i+1 == len(coordinates):
            orderLists.append("PosY")
            print("Route has been mapped")
            return orderLists
        else:
            currPos = coordinates[i]
            futPos = coordinates[i+1]
            step = moveChecker(currPos, futPos)
            orderLists.append(step)

UDP_IP_ADDRESS = laptopIP  # IP address of the Arduino board
UDP_PORT_Ard = 4000                 # port number for UDP communication
arduinoIpAddress = laptopIP # replace with your Arduino's IP address
CommandPort = 4000 # replace with the port number you want to use
P2Acoms = (Aip, CommandPort)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP_ADDRESS, UDP_PORT_Ard))

arduinoOrders = runAStar()
arduinoCommands = createCommands(arduinoOrders)
print(arduinoCommands)
i = 0
currentCommand = True

while i < len(arduinoCommands):
    data = arduinoCommands[i]
    if currentCommand:
        print(i)
        print(arduinoCommands[i])
        sock.sendto(data.encode(), P2Acoms)
        currentCommand = False
    
    recData, addr = sock.recvfrom(1024)
    print("Received message: ", recData.decode())
    
    if recData.decode().lower() == "next":
        i += 1
        print(i)
        currentCommand = True


# print("You have arricved at your destination")

