#include "leaplistener.h"
#include "serialArd.h"

/*
* Functions for establishing a Listener.
* Implementation from the class LeapListener
*/

#define DEMO_GESTURE	1
#define DEMO_TOUCHLESS	2
#define DEMO_GRADIENT	3
#define DEMO_SPHERE		4

void LeapListener::onInit(const Controller& leap)
{
	cout << "LeapMotion Initialized" << endl;

	//run app in background
	leap.setPolicyFlags(Leap::Controller::POLICY_BACKGROUND_FRAMES);
}

void LeapListener::onConnect(const Controller& leap)
{
	cout << "LeapMotion Connected" << endl;
	leap.enableGesture( Gesture::TYPE_SCREEN_TAP );
	//leap.enableGesture( Gesture::TYPE_KEY_TAP );
	leap.enableGesture( Gesture::TYPE_CIRCLE );
	//leap.enableGesture( Gesture::TYPE_SWIPE );
}

void LeapListener::onDisconnect(const Controller& leap)
{
	cout << "LeapMotion Disconnected" << endl;
}

void LeapListener::onExit(const Controller& leap)
{
	cout << "LeapMotion Exited" << endl;
}

/*
* Here we get the information from what is happening on
* the Leap. We're going to do the writing to the Arduino here.
* This is the best and easiest way to send data to it.
*/
void LeapListener::onFrame(const Controller& leap)
{
	// Get the most recent frame and report some basic information
	const Frame f = leap.frame();
	//gesture demos
	uint8_t dc = 0;
	dc = demo(DEMO_SPHERE, f);
	bool dontcare = writeArduino(dc, 0);
}

bool LeapListener::writeArduino(uint8_t val, uint8_t s)
{
	//make sure port is open
	if(arduino_fd <= 0)
		return false;

	int length = sizeof(val), nbytes;

	//write to Arduino
	nbytes = write(arduino_fd, &val, length);

	//write failed
	if(nbytes != length)
		return false;

	//wait for ack
	uint8_t ack = 0;

    do
    { 
		//read a char at a time
        read(arduino_fd, &ack, 1);  
    }
    while(ack != val);

    cout << "ACK: " << (unsigned int)ack << endl;

	return true;
}

uint8_t LeapListener::demo(int name, const Frame frame)
{

	uint8_t dc = 0;

	switch(name)
	{
		case DEMO_GESTURE :
		{
			// Get gestures
			const GestureList gestures = frame.gestures();

			Gesture gesture = gestures[0];

			switch (gesture.type())
			{
				//all we want is circle radius
				case Gesture::TYPE_CIRCLE :
				{
					CircleGesture circle = gesture;
					float r = circle.radius();
					cout << "circle radius: " << r << endl;

					dc = (uint8_t)(115 + 1.75 * (100 - (int)r));

					//handle overflow	
					if(dc < 70)					dc = 255;
					if(dc >= 70 && dc < 115)	dc = 115;

					break;
				}

				//we want nothing all we do is sned max dc
				case Gesture::TYPE_SCREEN_TAP :
				{
					cout << "screen tap!" << endl;
					dc = 255;
					break;
				}

				default : break;
		    }

			break;
		}

		case DEMO_TOUCHLESS :
		case DEMO_GRADIENT  :
		case DEMO_SPHERE	:
		{

			//location demos
			if (!frame.hands().isEmpty()) 
			{
				// Get the first hand
				const Hand hand = frame.hands()[0];

				// Check if the hand has any fingers
				const FingerList fingers = hand.fingers();

				//send change in duty cycle based on finger position
				if (!fingers.isEmpty())
				{
					//God this is ugly...
					float x = fingers[0].tipPosition().toFloatPointer()[0];
					float y = fingers[0].tipPosition().toFloatPointer()[1];
					float z = fingers[0].tipPosition().toFloatPointer()[2];
					cout << "X: " << x << endl;
					cout << "Y: " << y << endl;
					cout << "Z: " << z << endl;

					switch(name)
					{	
						// gradient feeling demo -- cool with Visualizer
						case DEMO_GRADIENT :
						{
							//z range is around -200 to 200
							//duty cycle is 115 to 255
							//o = o_min + ((o_max - o_min) / (i_max - i_min)) * (i - i_min))

							dc = 115 - 0.35 * (z - 200); 

							//just in case we go out of bounds
							if(dc < 70) dc = 255;
							else if(dc > 70 && dc < 110) dc = 110;

							cout << "dc: " << (int)dc << endl;
							break;
						}

						//intuitive feeling demo -- cool with Touchless for Mac
						case DEMO_TOUCHLESS :
						{
							
							if(frame.gestures()[0].type() == Gesture::TYPE_SCREEN_TAP)
								dc = 255;

							else
							{
								//beware of magic numbers!
								if(z > 70)						dc = 115-0.35 * (z - 200);
								else if(z <= 20 && z > -20)		dc = 255; //mid-holding state
								else if(z < 10)					dc = 250; //vibrate hand to death
							}
							break;
						}

						//touch a 3D sphere -- omgomgomg so cool!
						case DEMO_SPHERE :
						{
							//totally magic variable - xyz
							float center[3] = {0, 220, 0};
							float r = 50;

							float d = (x - center[0]) * (x - center[0]) +
										(y - center[1]) * (y - center[1]) +
										(z - center[2]) * (z - center[2]); 

							if(d <= (r * r))
								dc = 255;
							else
								dc = 0;

							cout << "d: " << d << endl;
							break;
						}
					}
				}
			}

			break;
		}

		default : break;
	}

	return dc;
}






		