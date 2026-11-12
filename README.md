# FPGA-based control systems

An experimental collection of DC motor control examples for an FPGA/Nios II environment, with a MATLAB serial integration example. The included PDF provides project background.

## Contents

- `MotorVelocityControl-NiosII.c`: Nios II motor velocity control code using hardware-specific timer, PWM, GPIO, and VGA interfaces.
- `MotorPositionControl.c`: position-control example communicating through standard input and output.
- `MotorAngle.m`: MATLAB model that exchanges values over a serial connection and plots the response.
- `FPGA_based_Control_Systems.pdf`: project document.

## Reproducing the experiment

The Nios II example depends on generated hardware definitions such as `system.h`, vendor libraries, and the associated FPGA design; these are not supplied as a complete build environment here. The MATLAB example uses the legacy `serial` API and a hard-coded serial port. Inspect and adapt the configuration for your hardware and MATLAB version.

The sources are experimental and may require fixes before compilation or execution. Review the controller, hardware connections, limits, and emergency-stop arrangements before operating a physical motor; start with simulation or an appropriately supervised test setup.
