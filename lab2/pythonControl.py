import msvcrt
import serial
ser = serial.Serial('COM5', 9600)
while 1:
    # Poll keyboard
    if msvcrt.kbhit():
        key = msvcrt.getch()
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
        elif key == b'u':
            ser.write(str.encode('C21UE'))
        elif key == b'd':
            ser.write(str.encode('C21DE'))
