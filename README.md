# 3D Printed Throttle
This is my 3D printed throttle. It is mostly 3D printed, and can be used to control a flight simulator.

It uses TinyUSB and the Pico SDK to read input from a potentiometer and send it too the computer. The potentiometer is twisted by a lever connected to the handle.

## Features
1. It is customizable. The handle can be changed, and the mounting plate has a hole for wires. It is also easy to add panels on top of the case for switches or dials.
1. It is very simple. Required it made up of few peices, requires few supports to print, and is easy to assemble.
1. Inexpensive. The total cost, without filament, should be no more then \$10 CAD. A pi pico costs about \$4, and it should be easy to find a potentiometer for less then \$6.

## Materials
1. Pi Pico, or other microcontroller.
1. Potentiometer
1. 7-9 M3 Screws. 
	+ Similar sizes would work too.
	+ The handle plate really only needs two screws, the ones through the levers.
1. 3 M3 nuts (or the screw size used)

## Instructions
1. Print all the components
1. Test your potentiometer with your microcontroller. You don't have to, but I would make sure it all works before soldering it all.
1. One of the supports has a single, larger hole in it. This hole should be drilled large enough that the potentiometer can spin freely.
1. One of the levers has a larger hole. This hole should be drilled until the potentiometers spinny bit can just fit, so it cannot move.
1. Assembly the the 3D printed parts. It should be easy to see how they connects, and _Throttle.FCStd_ show it all assembled and named.
1. Connect the potentiometer. The potentiometer needs to be all the way to one way when the throttle is all the way closed. **It is critical that the potentiometer signal pin is soldered to a pin that supports ADC. On the Pi Pico this is GPIO pins 26-29**
1. Upload code to the microcontroller. You can use the .uf2 I have but it probably won't work properly, because you will have to hardcode the min/max values of the potentiometer. Also only a max value works right now. See the code section for compiling and stuff.
1. You may have to tweak the potentiometer, because I found that when my potentiometer was all the way closed I would have to push the throttle a ways before is started to change. I fixed this by going in a game and rotationg the outside of the potentiometer until it read -1/1 only when the throttle was closed.
1. Glue the potentiometer in place. You can glue it real good and hope it never drifts, but I left it kinda loose so I can fix it easier. I used hot glue.

## Code
The code is very simple. It reads the potentiometer input from a ADC pin, as a 12-bit unsigned int. It then converts it to an 8-bit signed it, and scales it using the max value defined at the top of the file.

To define the max value use the constant at the top of the file. To change the pin, set the gpio pin value in the `input_pins` struct.

### Compiling
Compiling the code is extremely difficult. First install the pico sdk (https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html). Then the compile the code using the commands.

```cmake -B build/```

```cmake --build build/```

The `.uf2` file will be in build/src/

To install the code on the pico, plug it into your computer while holding the button. It will appear as a USB drive, just copy the binary to it.