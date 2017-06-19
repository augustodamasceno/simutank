# -*- coding: utf-8 -*-
#!/usr/bin/python

# PID controller example.

import socket
import time
import pid
import sys

# PID Configuration
channel = 0
setPoint = float(sys.argv[1]) 
pv = 0.0
mv = 0.0
integral_ = 0.0
sampleTime = 0.1
error = setPoint-pv
errorPrevious = 0.0
kp = 2.0
ki = 0.05
kd = 0.005
kwd = 0.0

# Manipulated Variable
def setMV():
    global sock,mv
    msg = 'WRITE %d %f\n' % (channel, mv)
    sock.sendall(msg)
    ack = sock.recv(64)

# Process Variable
def getPV():
    global sock,pv
    try:
        msg = 'READ %d' % channel
        sock.sendall(msg)
        pv = float(sock.recv(64))*6.25
    except socket.error, (value,message):
        sock.close()
        print "Error socket receiving. " + message
        connected = False

# Socket
ip = '127.0.0.1'
port = 20081
try:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((ip, port))
    print "Connected with Simutank!\n"
except socket.error, (value,message):
    sock.close()
    print "Error opening socket. " + message

while True: 
    integral_ = pid.integral(error,sampleTime,integral_,ki)
    mv = pid.pid(error,errorPrevious,sampleTime,integral_,kp,ki,kd)
    setMV()
    getPV()
    errorPrevious = error
    error = setPoint-pv
    time.sleep(sampleTime)

