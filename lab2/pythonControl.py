import msvcrt
import serial
ser = serial.Serial('COM5', 9600)
while 1:
    # Poll keyboard
    if msvcrt.kbhit():
        key = msvcrt.getch()
        if key == b'f':
            ser.write(str.encode('C21FE'))
        elif key == b's':
            ser.write(str.encode('C21SE'))
        elif key == b'l':
            ser.write(str.encode('C21LE'))
        elif key == b'r':
            ser.write(str.encode('C21RE'))
        elif key == b'b':
            ser.write(str.encode('C21BE'))
