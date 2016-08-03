#simutank
Simulator for Quanser's Coupled Tanks 

The interface is compatible with the server used in course  
"SISTEMAS DE CONTROLE (DCA0206)" of:  


  Department of Computer Engineering and Automation - DCA  
  Center of Technology - CT  
  Federal University of Rio Grande do Norte - UFRN, Natal, Brazil    

#Configuration  
** To configure the simulator, see line 30 and following lines.  
** The system is multi user, set at least maxClients to 2 if you want  
to control and use de GUI.  

#GUI  
** C code, needs pthread and OpenGL.  
  
#Communication  
**READ voltage. The Pressure sensor sensitivity is 6.25 cm/V**  
**WRITE voltage**  
  
Reading channel 0 (Level Tank 1)  
  "READ 0\n"  
  **Everything ok, you will receive "%f\n"**  
  **Otherwise "WRG\n"**  
Reading channel 1 (Level Tank 2)  
  "READ 1\n"  
  **Everything ok, you will receive "%f\n"**  
  **Otherwise "WRG\n"**  
Writing channel 0 (pump)  
  "WRITE 0 %f\n"  
  **Everything ok, you will receive "ACK\n"**  
  **Otherwise "WRG\n"**  
  
#Log

The files "logInput", "logOutput1" and "logOutput2" are generated by the SIGINT signal.  

#Plot

See examples for plot with Octave, Scilab, Python and Matlab in "plot" directory.  

##Supervisory Control System for Quanser's Coupled Tanks

https://github.com/augustomatheuss/ctank
