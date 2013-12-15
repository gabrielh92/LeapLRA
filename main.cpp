
#include "leaplistener.h"
#include "serialArd.h"
#include <iostream>

using namespace std;
using namespace Leap;

int main()
{
	//initialize Arduino Serial Port
	serialArd serial;

	int fd = serial.openPort();
	//end Arduino Serial Port initialization

	if(fd <= 0)
	{
		cout << "ERROR: Invalid File Descriptor." << endl;
		return 0;
	}

	//start LeapMotion initialization
	LeapListener listener = LeapListener(fd);
	Controller controller;

	controller.addListener(listener);
	//end LeapMotion initialization

	//gather LeapMotion data and write to ASP
	while(cin.peek() == EOF)
	{
		listener.onFrame(controller);
	}

	//remove LeapMotion listener
	controller.removeListener(listener);

  	//close Arduino Serial Port
  	serial.closePort();

	return 0;
}
