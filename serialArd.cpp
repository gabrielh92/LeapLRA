#include "serialArd.h"


#define USB1 	"/dev/tty.usbmodemfa141"
#define USB2	"/dev/tty.usbmodemfd131"

serialArd serialArd::operator=(const serialArd& rhs)
{
	isOpen = rhs.isOpen;
	_fd = rhs._fd;
	timeout = rhs.timeout;
	prevtio = rhs.prevtio;
	return *this;
}

/*
 * Function that opens the Serial Port on which the Arduino is connected.
 * Returns false on failure and true on success!
 */
int serialArd::openPort()
{
	struct termios tio;
	int fd;

	if(PathExists(USB1))
	{
		fd = open(USB1, O_RDWR | O_NOCTTY | O_NDELAY);
		cout << "Open Arduino on path: " << USB1 << endl;
	}
	else if(PathExists(USB2))
	{
		fd = open(USB2, O_RDWR | O_NOCTTY | O_NDELAY);
		cout << "Open Arduino on path: " << USB2 << endl;
	}
	else
	{
		cout << "Could not find Arduino." << endl;
		return -1;
	}

	//check we opened the port
	if(fd == -1)
	{	
		cout << "Port open failed." << endl;
		return -1;
	}

	cout << "Open in fd: " << fd << endl;

	//get the attributes
	if(tcgetattr(fd, &tio) < 0)
	{
		perror("Couldn't get attributes");
		return -1;
	}

	prevtio = tio;
	cfsetispeed(&tio, B115200); //B115200
	cfsetospeed(&tio, B115200); //B115200

    tio.c_cflag |= (CLOCAL | CREAD);
    tio.c_cflag &= ~PARENB;
    tio.c_cflag &= ~CSTOPB;
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;
    tio.c_cflag &= ~CRTSCTS;

	tcflush(_fd, TCIFLUSH); // flush pending transfers 

    //set the attributes
	if(tcsetattr(fd, TCSANOW, &tio) < 0)
	{
		perror("Couldn't set attributes");
		return -1;
	}

	//load status
	int status;
	ioctl(fd, TIOCMGET, &status);

	//disable DTR
	status &= ~TIOCM_DTR;

	//update the status
	ioctl(fd, TIOCMSET, &status);

	//initialize constants
	_fd = fd;
	isOpen = true;

	sleep(2);

	/*

	char buf[32], b[1];
	int i = 0;

	//wait cuz Arduino is stupid
	sleep(2);
	int cnt = 0;

	//hold until we read the \n from Arduino
	do
	{
		cnt++;

		if(cnt == 64)
		{
			cout << "Try power cycling Arduino" << endl;
			return -1;
		}

		//read 1 char
		int nbytes = read(fd, b, 1);

		//read failed
		if(nbytes == -1)
			return -1;

		//wait a little and try again
		if(nbytes == 0)
		{
			usleep(1000);
			continue;
		}

		buf[i] = b[0];
		i++;
	}
	while(b[0] != '\n');

	std::string s(buf);
	cout << s.substr(s.find("A"), s.find(".")) << endl;
*/
	return fd;
}

/*
 * Function that sends frequency, duty cycle and select information to
 * Arduino. Must succeed in all sends, else return false.
 * Right now, all we need to send is the duty cycle since it's the
 * only important variable for the project
 * However, it can be expanded to support frequency and output select
 */
bool serialArd::sendArduino(float dc)
{
	int length = sizeof(dc), nbytes;

	//make sure port is open
	if(!this->isOpen)
		return false;

	nbytes = write(_fd, &dc, length);

	//write failed
	if(nbytes != length)
		return false;

	//wait for ack
	uint8_t ack = 0;

    do
    { 
		//read a char at a time
        read(_fd, &ack, 1);  
    }
    while(ack != dc);

    cout << "ACK: " << (unsigned int)ack << endl;

	return true;
}

/*
 * Function that closes the Serial Port on which the Arduino is connected.
 * Returns false on failure and true on success!
 */
bool serialArd::closePort()
{
	if(!this->isOpen)
		return false;

	//turn the motor off
	sendArduino(0);

	//reset the serial port settings
	tcflush(_fd, TCIFLUSH); // flush pending transfers 
	tcsetattr(_fd,TCSANOW, &prevtio); // oldtio stores old serial settings 

	//close the fd
	close(_fd);

	return false;
}

bool serialArd::PathExists(string path)
{
	//path exists
   	if(access(path.c_str(), 0) == 0)
   	{
    	struct stat status;
    	stat(path.c_str(), &status);

    	//path is directory
    	if(status.st_mode & S_IFDIR)
      		return false;
      	//path is file -- so tty
      	else
      		return true;
   }
   //path doesnt exist
   else
	   return false;
}
