# CubeSpinner
Rubik's Cube solving robot using a Lego Mindstorms EV3 kit.

My second fully iteration of the cube robot. This time, the processing power 
of the EV3 intelligent brick allowed me to use a much better solving algorithm.
Additionally the new motor that comes with the EV3 kit runs much fast than the
other mindstorms motor, allowing a faster turning mechanic.

To achieve this speed, I had to modify the interface directly with the lego 
motor axis. This could be considered cheating, but it works nicely.
The programming is done in C with a GCC tool chain that was adopted for the
EV3 by John Hansen and I had to extend the sensor functionality to support
the color sensors.

For anyone who wants to build the cube spinner, the 'release' folder
contains the necessary instructions and the building plan.

There is also a video on youtube:

	https://www.youtube.com/watch?v=zQR2UfwRlkA

	