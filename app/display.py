#!/usr/bin/env python

from datetime import datetime, timedelta
import serial
import sys
import time


def connect_to_display(path):
    ser = serial.Serial(path, 19200)
    return ser

def main(argv):

    if len(argv) < 2:
        print "Usage: display.py serial_device str"
        exit(-1)

    ser = connect_to_display(argv[0])
    #time.sleep(5) 
    ser.write("M%s\n" % argv[1])
    #ser.write("T10\n")

if __name__ == "__main__":
   main(sys.argv[1:])

