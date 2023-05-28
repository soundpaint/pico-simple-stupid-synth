#!/bin/sh

# Use the minicom program to print out the Pico's debugging output on
# the Raspberry Pi's screen -- either by communication via USB
# (default) or via serial port (if the USB port is already used
# otherwise, e.g. for audio transmission).  Use keys "CTRL-A" and "X"
# to exit minicom.
#
# NOTE: Assuming Raspberry Pi 3 or 4, for communication via serial
# port, make sure via Preferences -> Raspberry Pi Configuration ->
# Interfaces, that Serial Port is enabled on the Raspberry Pi (or add
# the line "enable_uart=1" to file /boot/config.txt).  Alternatively,
# call the raspi-config program, select "Interface Options" ->
# "Serial", select "No" when asked "Would you like a login shell to be
# accessible over serial?" and "Yes" when asked "Would you like the
# serial port hardware to be enabled?".
#
# For communication via serial port, connect Raspberry Pi and Pico as
# follows:
#
# Pico ----------------------------------- Raspberry Pi
#
# GPIO 0 (UART0 TX, Pin 1) --------------- GPIO 14 (UART0 RX, Pin 10)
# GPIO 1 (UART0 RX, Pin 2) --------------- GPIO 15 (UART0 TX, Pin 8)
# GND (Pin 3) ---------------------------- GND (Pin 9)
#

# minicom via USB port
#minicom -b 115200 -D /dev/ttyACM0

# minicom via serial port
minicom -b 115200 -o -D /dev/ttyS0
