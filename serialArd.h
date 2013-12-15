#ifndef _SERIALARD_H
#define _SERIALARD_H

#include <string>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <stdint.h>
#include <sys/ioctl.h>
#include "leaplistener.h"

using namespace std;

/*
 * We need a Listener class in order to enable the gestures that we
 * want recognized and initialize the Leap.
 * We also use the Listener class to communicate with the Leap.
 * ... which is all of them
 */
class serialArd {

	public:

		/* opens the serial port for the Arduino and returns the fd*/
		int openPort();

		/* send values to Arduino */
		bool sendArduino(float dc);

		/* closes the serial port for the Arduino */
		bool closePort();

		/* assignment operator */
		serialArd operator=(const serialArd& rhs);

	private:

		bool isOpen; //just to make sure the port is open before sending data
		int _fd; //holds the fd on which the Arduino is connected
		int timeout; //to timeout on reads
		struct termios prevtio; //previous serial port parameters

		//helper to check if a path exists
		bool PathExists(string path);

};

#endif
