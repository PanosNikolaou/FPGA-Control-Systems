# FPGA-Control-Systems

In this repository are included the files for 
(1) A real-time machine control application on a FPGA based motor driver with PID (Proposal - Integral - Derivative)
controller for the velocity control of a DC Gear Motor connected through expansion header [JP2-GPIO 1] at Altera DE1 Board.
The PID controller has been implemented at the Cyclone II FPGA and the embedded microprocessor NIOS II (SoPC - System on Programmable Chip technology) with programming language C. The motor driver (velocity variations) has been programmed by the Pulse Width Modulation (PWM) technique and driven with a L293D H -Bridge. The frequency method has been used as the RPM feedback measurement technique from the decoded measurements of the optical encoder (E4P) of the DC Gear Motor. 
(2) A NIOS II 2nd order system simulation of a DC Motor position control with PID controller connected through
UART to a Personal Computer running Matlab and sending the unit step function as the desired position of the motor, (3) A
NIOS II 1st order system simulation of a white noise filter to a noisy signal transferred through UART by Matlab software.
