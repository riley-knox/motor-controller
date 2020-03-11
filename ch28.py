#!/usr/bin/env python3

from __future__ import print_function
import serial
ser = serial.Serial('/dev/ttyUSB0',230400,rtscts=1)
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\ta: Read Current Sensor Count \tb: Read Current Sensor (mA)'
          '\n\tc: Read Encoder Count \t\td: Read Encoder Count (degrees)'
          '\n\te: Reset Encoder Count \t\tq: Quit')
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'

    # send the command to the PIC32
    ser.write(selection_endline.encode()) # .encode() turns the string into a chararray
    # take the appropriate action
    # there is no switch() in python, using if elif instead
    if selection == 'a':
        # get current counts
        current_counts = int(ser.read_until(b'\n'))
        print('The motor current is ' + str(current_counts) + ' ADC counts.\n')

    elif selection == 'b':
        # get current value in mA
        current_amps = float(ser.read_until(b'\n'))
        print('The motor current is ' + str(current_amps) + ' mA.\n')

    elif selection == 'c':
        # get encoder counts function
        count_int = int(ser.read_until(b'\n'))
        print('The motor angle is ' + str(count_int) + ' counts.\n')

    elif selection == 'd':
        # example operation
        print('Positive angles are counter-clockwise.\n')
        count_str = ser.read_until(b'\n')  # get the incremented number back
        count_float = float(count_str) # turn it into an int
        print('The motor angle is ' + str(count_float) + ' degrees.\n') # print it to the screen

    elif selection == 'e':
        # reset encoder count
        print('Encoder count reset.\n')

    elif selection == 'q':
        print('Exiting client')
        has_quit = True # exit client
        # be sure to close the port
        ser.close()

    else:
        print('Invalid Selection ' + selection_endline)
