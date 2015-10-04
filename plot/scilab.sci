tank1 = read('../logOutput1',-1,1);
tank1 = tank1*6.25;  

plot(tank1,'linewidth',2);
title('Level Tank 1(cm) x time(cycles)')
xlabel('time(cycles)')
ylabel('Level(cm)')
