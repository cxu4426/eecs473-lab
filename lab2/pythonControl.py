import msvcrt
import serial
ser = serial.Serial('COM5', 9600)

speed = 75
length = 1

mode = 0 # 0: drive, 1: display

"""
Packet format:
    C    (start of message)
    #    (length of message in chars)
    #    (command [0:3])
    TEXT (stuff you are sending)
    E    (end of packet)
"""

print("Entering drive mode (mode 0)")

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
                print("turbo mode")
                speed = min(speed + 25, 150)
                print(speed)
            elif key == b'-':
                ser.write(str.encode('C21DE'))
                print("snail mode")
                speed = max(speed - 25, 0)
                print(speed)
            elif key == b'`': # stop driving and switch to display mode
                ser.write(str.encode('C21SE'))
                mode = 4
                buffer = []
                print("Entering display mode (mode = 4)")
        elif mode == 4: # display mode
            if key == b'\r': # enter key
                print("".join(buffer))
                packet = f'C{length}4{"".join(buffer)}E'
                        
                ser.write(str.encode(packet))
                print(f"Sent packet: {packet}")
                
                # clear buffer after sending
                buffer = []
                length = 1
            elif key == b'`':  # switch to drive mode
                mode = 0
                print("Entering drive mode (mode = 0)")
            else: # collect keyboard input in buffer
                if len(buffer) < 8:
                    buffer.append(key.decode())
                    length = length + 1
                else:
                    print("Buffer is full (max 8 chars)! Press enter to send.")
