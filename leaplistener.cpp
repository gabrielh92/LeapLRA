#include "leaplistener.h"
#include "serialArd.h"

/*
* Functions for establishing a Listener.
* Implementation from the class LeapListener
*/

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
	const Frame frame = leap.frame();

//location demos
//#if 1
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

			//cool sphere demo

			//sphere center at (0.0, 250.0, -50)
			//radius of r = 10	
			
		}
	}
/*
		// gradient feeling demo -- cool with Visualizer
		#if 0
			//z range is around -200 to 200
			//duty cycle is 115 to 255
			//o = o_min + ((o_max - o_min) / (i_max - i_min)) * (i - i_min))

			uint8_t dc = 115 - 0.35 * (z - 200); 
			uint8_t s = 1; //for now

			//just in case we go out of bounds
			if(dc < 70) dc = 255;
			else if(dc > 70 && dc < 110) dc = 110;

			cout << "dc: " << (int)dc << endl;

			//writeArduino(0x02);
			writeArduino(dc, s);

		//intuitive feeling demo -- cool with Touchless for Mac
		#else
			uint8_t dc = 0;
			
			if(frame.gestures()[0].type() == Gesture::TYPE_SCREEN_TAP)
			{
				dc = 255;
			}
			else
			{
				//beware of magic numbers!
				//if(z > 70)						dc = 115-0.35 * (z - 200);
				if(z <= 20 && z > -20)		dc = 255; //mid-holding state
				//else if(z < 10)					dc = 250; //vibrate hand to death
			}

			writeArduino(dc, 0);
		#endif
	
		}
	}

//gesture demos
#else

	uint8_t dc = 0;

	// Get gestures
	const GestureList gestures = frame.gestures();

		Gesture gesture = gestures[0];

		switch (gesture.type())
		{
			//all we want is circle radius
			case Gesture::TYPE_CIRCLE:
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
			case Gesture::TYPE_SCREEN_TAP:
			{
				cout << "screen tap!" << endl;
				dc = 255;
				break;
			}
			default:
		        break;
		}

		writeArduino(dc);
#endif
*/
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

