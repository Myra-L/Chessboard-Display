# Programmer: Christian Escutia
# ~ChessBoard gamme Logic on a Raspberry Pi4 with Arduino Support

#imports
import serial
import RPi.GPIO as GPIO
import time

ser=serial.Serial("/dev/ttyACM0",9600)  #change ACM number as found from ls /dev/tty/ACM*
ser.baudrate=9600

#global variables
# creating list for tiles
list = []
button = 0
pos1 = 0
pos2 = 0

def GameSetup():
    print ("Begining Chess Game Setup...")

    #start arrays to hold game board data
    class tile:  
        def __init__(self, number, troop, player):  
            self.number = number  
            self.troop = troop
            self.player = player

    # appending instances to list  
    # player 1 side
    list.append( tile(1, "rook", 1) )
    list.append( tile(2, "knight", 1) )
    list.append( tile(3, "bishop", 1) )
    list.append( tile(4, "king", 1) )
    list.append( tile(5, "queen", 1) )
    list.append( tile(6, "bishop", 1) )
    list.append( tile(7, "knight", 1) )
    list.append( tile(8, "rook", 1) )
    list.append( tile(9, "pawn", 1) )
    list.append( tile(10, "pawn", 1) )
    list.append( tile(11, "pawn", 1) )
    list.append( tile(12, "pawn", 1) )
    list.append( tile(13, "pawn", 1) )
    list.append( tile(14, "pawn", 1) )
    list.append( tile(15, "pawn", 1) )
    list.append( tile(16, "pawn", 1) )

    # other tiles
    for x in range(17, 49):
        list.append( tile(x, "none", 0) )

    # player 2 side
    list.append( tile(49, "pawn", 2) )
    list.append( tile(50, "pawn", 2) )
    list.append( tile(51, "pawn", 2) )
    list.append( tile(52, "pawn", 2) )
    list.append( tile(53, "pawn", 2) )
    list.append( tile(54, "pawn", 2) )
    list.append( tile(55, "pawn", 2) )
    list.append( tile(56, "pawn", 2) )
    list.append( tile(57, "rook", 2) )
    list.append( tile(58, "knight", 2) )
    list.append( tile(59, "bishop", 2) )
    list.append( tile(60, "king", 2) )
    list.append( tile(61, "queen", 2) )
    list.append( tile(62, "bishop", 2) )
    list.append( tile(63, "knight", 2) )
    list.append( tile(64, "rook", 2) )

    print ("Setup Complete...")
    print("Game Start...")

def WinCon():
    count = 0
    for x in range(64):
        if(list[x].troop == "king"):
            count = count + 1
    if(count != 2):
        print("Game Over Reset...")
        time.sleep(5)
        GameSetup()

def ButtonFinder(tilenum):
    if(button == 0):
        pos1 = tilenum
        button = button + 1
    if(button == 1):
        pos2 = tilenum
        button = button - 1

def Move()
    print("tile %s moved to -> %s" % (pos1, pos2))
    list[pos2].player = list[pos1].player
    list[pos2].troop = list[pos1].player

    list[pos1].player = 0
    list[pos1].troop = "none"


#begining of Game Logic
#initial setup before forever loop
GameSetup()
while True:
    buttoncount = 0
    while True:
        if(read_ser=ser.readline()):
            print("Button Pressed -> %s" % (read_ser))
            ButtonFinder(read_ser)
            buttoncount = buttoncount + 1
        if(buttoncount == 2)
            break
    Move()
    WinCon()
