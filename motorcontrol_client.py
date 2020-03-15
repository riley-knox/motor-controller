#!/usr/bin/env python3

from __future__ import print_function
import serial
import matplotlib.pyplot as plt

# create and open serial port object
ser = serial.Serial('/dev/ttyUSB0',230400,rtscts=1)
print('Opening port: ')
print(ser.name)

# helper function to plot trajectories
# def plot_trajectory():


has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\ta: Read Current Sensor Count \tb: Read Current Sensor (mA)'
          '\n\tc: Read Encoder Count \t\td: Read Encoder Count (degrees)'
          '\n\te: Reset Encoder Count \t\tf: Set PWM (-100 to 100)'
          '\n\tg: Set Current Gains \t\th: Get Current Gains'
          '\n\ti: Set Position Gains \t\tj: Get Position Gains'
          '\n\tk: Test Current Control'
          '\n\tp: Unpower the Motor \t\tq: Quit'
          '\n\tr: Get Operating Mode')
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'

    # send the command to the PIC32
    ser.write(selection_endline.encode()) # .encode() turns the string into a chararray

    # take the appropriate action
    if selection == 'a':            # get current counts
        current_counts = str(int(ser.read_until(b'\n')))        # read motor current (counts)
        print('The motor current is ' + current_counts + ' ADC counts.\n')

    elif selection == 'b':          # get current value in mA
        current_amps = str(float(ser.read_until(b'\n')))        # read motor current (mA)
        print('The motor current is ' + current_amps + ' mA.\n')

    elif selection == 'c':          # get encoder counts
        count = str(int(ser.read_until(b'\n')))                 # read angle (counts)
        print('The motor angle is ' + count + ' counts.\n')

    elif selection == 'd':          # get encoder degrees
        print('Positive angles are counter-clockwise.\n')
        count = str(float(ser.read_until(b'\n')))               # read angle (degrees)
        print('The motor angle is ' + count + ' degrees.\n')

    elif selection == 'e':          # reset encoder count
        print('Encoder count reset.\n')

    elif selection == 'f':          # set PWM duty cycle
        duty_cycle = input('Enter duty cycle as a percentage: ')        # set PWM duty cycle
        ser.write((duty_cycle+'\n').encode())                           # serial write

        if int(duty_cycle) < 0:                 # spin direction
            spin = '(clockwise).\n'
        elif int(duty_cycle) >= 0:
            spin = '(counter-clockwise).\n'

        print('Duty cycle set to ' + str(abs(int(duty_cycle))) + '% of maximum ' + spin)

    elif selection == 'g':          # set current gains
        K_p = input('Enter your desired Kp current gain: ')         # set Kp, Ki
        K_i = input('Enter your desired Ki current gain: ')
        print('Sending Kp = ' + K_p + ' and Ki = ' + K_i + ' to the current controller.\n')
        ser.write((K_p + '\n').encode())                            # write to PIC
        ser.write((K_i + '\n').encode())

    elif selection == 'h':          # get current gains
        K_p = str(float(ser.read_until(b'\n')))             # serial read to get Kp & Ki
        K_i = str(float(ser.read_until(b'\n')))
        print('The current controller is using Kp = ' + K_p + ' and Ki = ' + K_i + '.\n')

    elif selection == 'i':          # set position gains
        K_p = input('Enter your desired Kp position gain: ')
        K_i = input('Enter your desired Ki position gain: ')
        K_d = input('Enter your desired Kd position gain: ')
        print('Sending Kp = ' + K_p + ', Ki = ' + K_i + ', and Kd = ' + K_d + ' to the position controller.\n')
        ser.write((K_p + '\n').encode())
        ser.write((K_i + '\n').encode())
        ser.write((K_d + '\n').encode())

    elif selection == 'j':          # get position gains
        data_read = ser.read_until(b'\n')
        data_text = str(data_read, 'utf-8')
        data = list(map(float, data_text.split()))

        Kp, Ki, Kd = str(data[0]), str(data[1]), str(data[2])

        print('The position controller is using Kp = ' + Kp + ', Ki = ' + Ki + ', and Kd = ' + Kd + '.\n')

    elif selection == 'k':          # test current control
        print("Testing current control. Press 'q' to close plot.\n")

        time_vec = []                           # list of time points
        current_ref = []                        # reference current values
        current_meas = []                       # measured current values

        num_points = int(ser.read_until(b'\n'))      # number of points to plot

        # acquire points to plot
        for i in range(num_points):
            time_vec.append(i*0.2)              # time point i

            data_read = ser.read_until(b'\n')
            data_text = str(data_read, 'utf-8')
            data = list(map(int, data_text.split()))
            if len(data) == 2:
                current_ref.append(data[0])             # add reference current value
                current_meas.append(data[1])            # add measured current value

        # create and show plot
        plt.plot(time_vec, current_ref, 'b-', label='Measured Current')
        plt.plot(time_vec, current_meas, 'r-', label='Reference Current')
        plt.xlabel('Time (ms)')
        plt.ylabel('Current (mA)')
        plt.title('Current Feedback Control')
        plt.legend(loc='best')
        plt.show()

    # elif selection == 'l':          # go to angle (deg)

    # elif selection == 'm':          # load step trajectory

    # elif selection == 'n':          # load cubic trajectory

    # elif selection == 'o':          # execute trajectory

    elif selection == 'p':          # turn motor off
        print('Turned off motor power.\n')

    elif selection == 'q':          # quit
        print('Exiting client')
        has_quit = True             # exit while loop
        ser.close()                 # close serial port

    elif selection == 'r':          # return operating mode
        op_modes = ['IDLE', 'PWM', 'ITEST', 'HOLD', 'TRACK']    # operating modes
        mode = op_modes[int(ser.read_until(b'\n'))]             # get mode back
        print('Operating mode: ' + mode + '\n')

    else:                           # invalid selection
        print('Invalid Selection ' + selection_endline)
