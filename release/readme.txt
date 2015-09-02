How to create your own "CubeSpinner"

1. Build the mechanism

Use the provided Lego Digital Designer file as a building plan. Most parts are from the EV3 kit, 
with the exception of the white and yellow (color is optional)  round bricks. The second color sensor 
(the one that connects to port 4) is optional but will speed up the scanning process by 4 seconds. 
I could not draw the rubber bands with the designer program. These are needed to pull the two arms 
together and also to make the ratchet-mechanism work. On my youtube video
(https://www.youtube.com/watch?v=zQR2UfwRlkA) you can see the proper placement.

2. Prepare a cube 

I think you will need some speed cube because of its corner-cutting abilities and 
because there you can normally pull out the center caps to reach the adjustment screws.
I used the "Speed Cube Ultimate II" for this purpose which was suited perfectly. 
In place of the center caps put in two lego 2x2 plates with the studs outwards.
With my cube this worked like a charm - I only had to file away a tiny bit of the lego plate 
corners to make it fit. To make this more durable I glued the plates in, but this is optional if 
you do not want to change the cube permanently.
The color scheme of the cube must follow the standard rubiks cube with stickers/plates on
black background.

2. Install the software

Basically you have to create a new subfolder of "prjs" on the EV3 brick. 
Calling this folder "CubeSpinner" may be a good choice. Copy the files
"CubeSpinner.rbf" and "spinnermain.rbf" to this directory.

How you actually can transfer files largely depends on the software you are using.
I will explain how to do this with the standard lego mindstorms software:
  2.a. Connect the EV3 brick to your computer
  2.b. Start the lego development environment. 
  2.c. Create a new project named "CubeSpinner" which will contain just the start block.
  2.d. Download the program to the brick. This will create the necessary folder "CubeSpinner" 
        with a program file inside.
  2.e. Start up the memory browser. This will show you the new folder and the contents
  2.f. Delete the program file inside the CubeSpinner folder
  2.g.Select the "CubeSpinner" folder and use "Download" to transfer my provided.rbf files to the brick.

3. Operating the machine

Before you start the program, make sure all motor arms are well aligned. Use the two
red rotating parts on the outsides to feel with your fingertips if the alignment is centered.
The red parts should be able to move forward and backwards a bit by the same amount.
Likewise you can align the rotor with the red horizontal bar. 
Once the program runs, this alignment can not be changed.
  
Start the Action "CubeSpinner" from the Project "CubeSpinner".  Maybe you have
to move the button extension to the side to reach the navigation buttons.
After startup the program creates some large data tables which will take about 
1:40 minutes. After this the color sensor(s) light will turn white and the machine is ready 
to accept a cube.

Scramble the cube and insert it in correct orientation: White center must be left, 
yellow center must be right (as color coded on the machine) and the orange
center must be on top.

Press the button extension handle to start the run.

In case you have only one color sensor connected, the cube will be solved (if nothing 
goes wrong). For the case of two color sensors, the first run is done only to get 
a calibration of the color sensors and will leave the cube unchanged after some
scrambling and de-scrambling. Then press the start button a second time to 
actually do a solve. This calibration is necessary once after each start of the program.

After each sucessful solve, you can get some status information on the display about
the time used and the length of the solution.

4. Troubleshooting

The mechanism and the program were developed and tested with a 9V power
supply (some DC power adapter or non-rechargable batteries). I have tried it with 7.2V 
(rechargable batteries) also and it seemed to work just fine even if the motors
run a little slower. If you experience problems with this setup, please let me know.

The color sensor works best with dim light. I have seen that especially modern LED 
lamps cause real trouble to the color sensor. If the scan process fails to deliver a 
plausible color asignment, the brick gives a beep and writes a diagram of the scan result
on the screen. If scanning consistently fails for your cube even in dim conditions,
please also let me know.


have much fun with this design,
Reinhard Grafl
(reinhard.grafl@aon.at)
