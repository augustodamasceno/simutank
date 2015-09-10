import matplotlib.pyplot as plot

tank1File = open('../logOutput1', 'r')
tank1 = []
for line in tank1File:
	tank1.append(float(line))

plot.plot(tank1)
plot.xlabel('time(cycles)')
plot.ylabel('Level(cm)')
plot.title('Level Tank 1(cm) x time(cycles)')

plot.show()

tank1File.close()

