from struct import unpack 

import datetime as dt
import serial  
import os 

PATH = os.path.dirname( __file__ )
COMP = serial.Serial( '/dev/ttyUSB0', baudrate = 9600 )
medida, dTime = 0, 0 

with open(PATH + '\\Caminha.txt', 'w' ) as FILE:
    COMP.flushInput()
    while True: 
        while COMP.inWaiting() < 4:
            pass 
        Medida = unpack( '<f', COMP.read( 4 ) )[0]
        dTime =  dt.datetime.now()
        FILE.write( str(Medida) + ' ' + str(dTime) + '\n' )
        print( Medida, dTime )