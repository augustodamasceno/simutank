tank1 = dlmread('logOutput1');
tank1 = tank1*6.1;  

plot(tank1,'linewidth',2);
title('Level Tank 1(cm) x time(cycles)','fontsize', 14)
xlabel('time(cycles)','fontsize', 12)
ylabel('Level(cm)','fontsize', 12)
