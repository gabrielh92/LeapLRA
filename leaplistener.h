#ifndef _LEAPLISTENER_H
#define _LEAPLISTENER_H

#include "include/Leap.h"
#include "include/LeapMath.h"
#include "serialArd.h"
#include <iostream>

using namespace Leap;
using namespace std;

/*
 * We need a Listener class in order to enable the gestures that we
 * want recognized and initialize the Leap.
 * We also use the Listener class to communicate with the Leap.
 * ... which is all of them
 */
class LeapListener : public Listener {

	public:

		/* Constructor to define fd */
		LeapListener(int fd)
		{
			arduino_fd = fd;
		}

		/* Runs when the Leap is initialized - when the code runs */
		virtual void onInit(const Controller& leap);

		/* Runs when the Leap is connected */
		virtual void onConnect(const Controller& leap);

		/* Handles loose stuff when Leap is disconnected w/o real exit */
		virtual void onDisconnect(const Controller& leap);

		/* Runs when Leap is exited - Controller class itself handles everything */
		virtual void onExit(const Controller& leap);

		/* This is where we detect stuff - this executes at every frame iteration */
		virtual void onFrame(const Controller& leap);

	private:
		int arduino_fd; //we need to store the fd here cuz we cant write to it otherwise
		int numFrames; //count so we dont write to arduino every frame

		/* helper function to write to Arduino */
		bool writeArduino(uint8_t val, uint8_t s);
};

#endif

