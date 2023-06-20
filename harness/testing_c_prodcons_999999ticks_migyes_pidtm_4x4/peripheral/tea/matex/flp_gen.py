#!/usr/bin/python

import sys 

XX = int(sys.argv[1])
YY = int(sys.argv[2])

print("x: " + str(XX) + " y: " + str(YY))

# if((XX==2 and YY==2) or (XX==5 and YY==5) or (XX==11 and YY==11)):
# 	side = 0.000170
# elif ((XX == 9 and YY==9) or (XX==10 and YY==10)  or (XX==12 and YY==12)):
# 	side = 0.0002
# else:
# 	side = 0.000120#194

side = 0.000266 

with open('input/floorplan.flp', 'w') as file:

	for y in range(0,YY):
		for x in range(0,XX):

			#file.write('p' + str(y*XX+x) + '\t' + str(side) + '\t' + str(side) + '\t' + str(side*x)+ '\t' + str(side*y) + "\n")
			file.write('p' + str(y*XX+x) + '\t' + str("{:.8f}".format(side)) + '\t' + str("{:.8f}".format(side)) + '\t' + str("{:.8f}".format(side*x))+ '\t' + str("{:.8f}".format(side*y)) + "\n")

with open('input/power.pwr', 'w') as file:
	file.write('time ')
	for n in range(0,(YY*XX)):
		file.write('p' + str(n))
		if n <= (YY*XX-2):
			file.write(' ')
	file.write('\n0.0 ')
	for n in range(0,(YY*XX)):
		file.write('0')
		if n <= (YY*XX-2):
			file.write(' ')
	file.write('\n0.001 ')
	for n in range(0,(YY*XX)):
		file.write('0')
		if n <= (YY*XX-2):
			file.write(' ')