# -*- coding: utf-8 -*-
#!/usr/bin/python

##
#  Simulator for Quanser's Coupled Tanks
#  Copyright (C) 2015, Augusto Damasceno
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import thread
import time
import socket
import re

# Configure ip and port   
ip = '127.0.0.1'
port = 20081 

# Configure noise
# When noise is enabled, each noisePeriod seconds,
# a noise between 0% and noiseLevel% of noiseMax is added
# to the signal response
noise = False
noiseLevel = 0.01
noisePeriod = 0.01
noiseMax = 4.0

# Configure log
# If log is enabled, logInput and logOuput
# can be enabled independently 
log = False
logInput = False
logOutput = False

# Prints enabled/disabled
debug_mode = True

# Model
readChannel = [0.0,0.0]
writeChannel = 0.0
time_interval = 2
def model():
	global readChannel, writeChannel
	while 1:
		#calc readChannels from input
		readChannel[0] = time.time()
		if debug_mode:
			print "Calculating..."
		time.sleep(time_interval)

# Create model thread
try:
   thread.start_new_thread( model, () )
except:
   print "Thread Error!"

# Socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind((ip, port))

# Run server
while 1:
	sock.listen(1)
	conn, addr = sock.accept()

	if debug_mode:
		print 'Connected with: ', addr

	while 1:
		data = conn.recv(64)
		if not data: break
		if debug_mode:
			print "Received: ", data
		if "WRITE" in data:
			numbers = re.findall(r"[-+]?\d*\.\d+|\d+",data)
			if len(numbers) < 2:
				conn.send("WRG\n")
			else:	
				if int(numbers[0]) == 0:
					writeChannel = float(numbers[1])
					if debug_mode:
						print "Wrote to channel %d, voltage %f" \
							% (int(numbers[0]),float(numbers[1]))
					conn.send("ACK\n")
				else:
					conn.send("WRG\n")
		elif "READ" in data:
			numbers = re.findall(r"[-+]?\d*\.\d+|\d+",data)
			if len(numbers) > 0:
				readChannelstr = str(readChannel[int(numbers[0])]) 
				if debug_mode:
					print "Read from channel %d, voltage: %f" \
						% (int(numbers[0]),readChannel[int(numbers[0])])
				conn.send(readChannelstr)
	conn.close()

