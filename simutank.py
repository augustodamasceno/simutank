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
import numpy
import math
import signal

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
log = True
logInput = True
logOutputs = True
logIn = [0.0]
logOut1 = [0]
logOut2 = [0]

# Prints enabled/disabled
debug_mode = True

# Model
readChannel = [0,0]
writeChannel = 0.0
time_interval = 0.05
resolutionAD = 1023.0
maxAnalogValue = 4.8
constantAD = resolutionAD/maxAnalogValue
def model():
###
# Model reference from:
#
# Júnior, Francisco G. F.; Maitelli, André L.; 
# Lopes, José S. B.;Araújo, Fabio M. U.
# Oliveira, Luiz A. H. G. 
# IMPLEMENTAÇÃO DE CONTROLADORES PID UTILIZANDO
# LÓGICA FUZZY E INSTRUMENTAÇÃO INDUSTRIAL
# VII Simpósio Brasileiro de Automação Inteligente.
# São Luís, setembro de 2005
#
	global readChannel, writeChannel,logIn,logOut1,logOut2
	# Tank orifice diameter	(cm^2)
	a1 = 0.178
	a2 = a1
	# Tank base area (cm^2)
	A1 = 15.518
	A2 = A1
	# Gravitational acceleration (m/s^2)
	g = 9.81
	# Pump flow constant ((cm^3)/sV)
	km = 4.6
	# Voltage applied (V)
	writeChannel = 0.0
	# Level tank 1
	# x[1]
	# Level tank 2
	# x[2]

	# Operating Points 5cm and 25cm
	# Equation 1 (LaTeX):
	# \dot{L_{1}} = -\frac{a_{1}}{A_{1}}\sqrt{2gL_{1}}
	# +\frac{K_{m}}{A_{1}}V_{p}
	# Equation 2 (LaTeX):
	#  \dot{L_{2}} = -\frac{a_{2}}{A_{2}}\sqrt{2gL_{2}}
	# +\frac{a_{1}}{A_{2}}\sqrt{2gL_{1}}

    # State space
	A =  numpy.array([[(a1/A1)*math.sqrt(2*g), 0.0],\
		 [(a1/A2)*math.sqrt(2*g) , (a2/A2)*math.sqrt(2*g)]]) 
	Bt = numpy.array([(km/A1),0.0])
	xt = numpy.array([0.0,0.0])
    
	while 1:
		Ax = A.dot(xt.T)
		Bu = Bt.T*writeChannel
	
		# x* = Ax+Bu
		# x* = dx/dt = (x-x_past)/d_time		
		xt = xt + time_interval*(Ax+Bu.T)	
		
		# Prevent negative level
		if xt[0] < 0.0:
			xt[0] = 0.0
		if xt[1] < 0.0:
			xt[1] = 0.0

		# AD conversion
		readChannel[0] = math.ceil(constantAD*xt[0])
		readChannel[1] = math.ceil(constantAD*xt[1])

		if log:
			if logInput:
				logIn.append(writeChannel)
			if logOutputs:
				logOut1.append(readChannel[0])
				logOut2.append(readChannel[1])
		if debug_mode:
			print 'Pump: %f' %  writeChannel
			print 'Level 1: ' % xt[0]
			print 'Level 2: ' % xt[1]	
		time.sleep(time_interval)

# Create model thread
try:
   thread.start_new_thread( model, () )
except:
   print "Model Thread Error!"

# Save log
def handler(signum, frame):
	global logIn, logOut1, logOut2
	if log:
		if logInput:
			filelog = open('logInput', 'w');
			filelog.write(str(logIn))
			filelog.close()
		if logOutputs:
			filelog = open('logOutput1', 'w');
			filelog.write(str(logOut1))
			filelog.close()
			filelog = open('logOutput2', 'w');
			filelog.write(str(logOut2))
			filelog.close()
		print "\nLog saved!"

signal.signal(signal.SIGINT, handler)
	
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
						print "Wrote to channel %d, voltage %f\n" \
							% (int(numbers[0]),float(numbers[1]))
					conn.send("ACK\n")
				else:
					conn.send("WRG\n")
		elif "READ" in data:
			numbers = re.findall(r"[-+]?\d*\.\d+|\d+",data)
			if len(numbers) > 0 & \
				(int(numbers[0]) == 0 | int(numbers[0]) == 1):
				readChannelstr = str(readChannel[int(numbers[0])]) 
				readChannelstr = readChannelstr + '\n'
				if debug_mode:
					print "Read from channel %d, level: %d\n" \
						% (int(numbers[0]),readChannel[int(numbers[0])])
				conn.send(readChannelstr)
			else:
				conn.send("WRG\n")
		else:
				conn.send("WRG\n")
	conn.close()

