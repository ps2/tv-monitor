from flask import Flask
import time
import serial
import sys
from werkzeug.contrib.cache import MemcachedCache
from flask import render_template, redirect, url_for

cache = MemcachedCache()

SERIAL_PATH="/dev/ttyUSB0"

def connect_to_display(path):
    ser = serial.Serial(path, 19200)
    return ser

app = Flask(__name__)

def update_time_remaining(remaining):
    cache.set('screen_time_start', time.time())
    cache.set('screen_time_duration', remaining)
    ser = connect_to_display(SERIAL_PATH)
    ser.write("T%d\n" % remaining)

def get_time_remaining():
    elapsed = 0
    screen_time_start = cache.get('screen_time_start')
    if screen_time_start:
        elapsed = time.time() - screen_time_start

    screen_time_duration = cache.get('screen_time_duration') or 0
    remaining = screen_time_duration - elapsed
    
    if remaining < 0:
        remaining = 0
    return remaining

def format_remaining(remaining):
    if remaining > 60:
        return "%dm" % (remaining/60+1)
    else:
        return "%ds" % (remaining)

def update_message(message):
    ser.write("M%s\n" % message)

@app.route('/')
def index():
    remaining = get_time_remaining()
    return render_template("screentime.html", remaining=format_remaining(remaining))

@app.route('/add')
def add_ten_minutes():
    remaining = get_time_remaining()
    remaining += 10 * 60
    update_time_remaining(remaining)
    return redirect(url_for('index'))

@app.route('/subtract')
def subtract_ten_minutes():
    remaining = get_time_remaining()
    remaining -= 10 * 60
    update_time_remaining(remaining)
    return redirect(url_for('index'))

