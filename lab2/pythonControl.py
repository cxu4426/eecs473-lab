import msvcrt
import serial
ser = serial.Serial('COM5', 9600)

mode = 0 # 0: drive, 1: display

while 1:
    # Poll keyboard
    if msvcrt.kbhit():
        key = msvcrt.getch()
        if mode == 0: # drive mode
            if key == b'w':
                ser.write(str.encode('C21FE'))
            elif key == b' ':
                ser.write(str.encode('C21SE'))
            elif key == b'a':
                ser.write(str.encode('C21LE'))
            elif key == b'd':
                ser.write(str.encode('C21RE'))
            elif key == b's':
                ser.write(str.encode('C21BE'))
            elif key == b'=':
                ser.write(str.encode('C21UE'))
            elif key == b'-':
                ser.write(str.encode('C21DE'))
            elif key == b'`': # stop driving and switch to display mode
                ser.write(str.encode('C21SE'))
                ser.write(str.encode('C21`E')) # TODO: might not work :D
                mode = 1
        elif mode == 1: # display mode
            if key == '`':
                # clear display
                ser.write(str.encode(key)) # TODO: might not work :D
                mode = 0
            else while !<enter key>
                collect chars into buffer and then ser.write
