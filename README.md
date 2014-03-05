LeapLRA
=======

The Leap Motion - LRA system uses an Arduino and the Leap SDK to provide useful haptic feedback for the user of a Leap Motion so they can know what they're doing.

The necessary hardware is:
- Leap Motion
- Arduino
- TI DRV 2603 for driving haptic motors
- LRA Motors

Default demos include
	- Gestures Demo: different vibrations depending on gesture
	- Gradient Demo: feel a gradient of strength in vibration in the z-axis
	- Touchless for Mac (Windows untested) Demo: demo that makes using Touchless for Mac a little more intuitive
	- 3D Sphere Demo: demo where you can use your LRA connected finger to touch a sphere in the air
	- To add other demos, define them as a macro and add them into the demo function's switch statement at the end of leaplistener.cpp


	Have fun!