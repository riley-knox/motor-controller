#!/usr/bin/env python3

import serial
ser = serial.Serial('/dev/ttyUSB0',230400,rtscts=1)
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\tc: Read Encoder Count \td: Dummy Command'
          '\n\te: Reset Encoder Count\tq: Quit'
          '\n\tx: Sum') # '\t' is a tab
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'

    # send the command to the PIC32
    ser.write(selection_endline.encode()) # .encode() turns the string into a chararray
    # take the appropriate action
    # there is no switch() in python, using if elif instead
    if selection == 'c':
        # get encoder counts function
        count_int = int(ser.read_until(b'\n'))
        print('The motor angle is ' + str(count_int) + ' counts.\n')

    elif selection == 'd':
        # example operation
        n_str = input('Enter number: ') # get the number to send
        n_int = int(n_str) # turn it into an int
        print('number = ' + str(n_int)) # print it to the screen to double check

        ser.write((str(n_int)+'\n').encode()) # send the number
        n_str = ser.read_until(b'\n')  # get the incremented number back
        n_int = int(n_str) # turn it into an int
        print('Got back: ' + str(n_int) + '\n') # print it to the screen

    elif selection == 'e':
        # reset encoder count
        print('Encoder count reset.\n')

    elif selection == 'q':
        print('Exiting client')
        has_quit = True # exit client
        # be sure to close the port
        ser.close()

    elif selection == 'x':
        m_str = input('Enter first number: ')   # get first number to send
        m_int = int(m_str)                      # turn it into an int
        n_str = input('Enter second number: ')  # get second number to send
        n_int = int(n_str)                      # turn it into an int
        print('number 1 = ' + str(m_int))       # print numbers to screen
        print('number 2 = ' + str(n_int))

        ser.write((m_str+'\n').encode())        # send numbers to PIC32
        ser.write((n_str+'\n').encode())

        s_str = ser.read_until(b'\n')           # get sum back
        s_int = int(s_str)                      # turn sum into int

        print('Got back: ' + str(s_int) + '\n') # print to screen

    else:
        print('Invalid Selection ' + selection_endline)
