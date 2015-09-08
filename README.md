#simutank
Simulator for Quanser's Coupled Tanks 

The interface is compatible with the server used in course  
"SISTEMAS DE CONTROLE (DCA0206)" of:  


  Department of Computer Engineering and Automation - DCA  
  Center of Technology - CT  
  Federal University of Rio Grande do Norte - UFRN, Natal, Brazil  
  
#Communication  
**READ an AD**  
**WRITE voltage directly**  
  
Reading channel 0 (Level Tank 1)  
  "READ 0\n"  
  **Everything ok, will you receive "%f\n"**  
  **Otherwise "WRG\n"**  
Reading channel 1 (Level Tank 2)  
  "READ 1\n"  
  **Everything ok, will you receive "%f\n"**  
  **Otherwise "WRG\n"**  
Writing channel 0 (pump)  
  "WRITE 0 %f\n"  
  **Everything ok, will you receive "ACK\n"**  
  **Otherwise "WRG\n"**  
  
#Plot

See examples in Octave, Scilab, Python and Matlab in "plot" folder.  
