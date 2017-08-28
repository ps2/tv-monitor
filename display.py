#!/usr/bin/env python

from datetime import datetime, timedelta
import serial
import sys


def connect_to_sensor(path):
    ser = serial.Serial(path, 9600)
    ser.read()
    return ser

def main(argv):

    if len(argv) < 2:
        print "Usage: display.py serial_device str"
        exit(-1)

    ser = connect_to_display(argv[0])
    print version
    if not re.match(r'.*shoe.*', version):
        print "Only to be used with Sensei shoe firmware."
        exit(-1)

    ser.readline() # sensor id line

    id_to_write = int(argv[1])
    if id_to_write <= 0 or id_to_write >= 64:
        print "Id must be > 0 and < 64"
        exit(-1)

    ser.write("Z%02x" % id_to_write)
    time.sleep(0.1)
    ser.write("I")
    read_id = ord(ser.read(1))
    print "Set id to %d (0x%02x)" % (read_id, read_id)

if __name__ == "__main__":
   main(sys.argv[1:])

