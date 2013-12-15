/*
 * File into Arduino
 * For LeapMotion feedback through LRA Motors
 * Drives one LRA Motor connected to the TI 2603 driver on RIGHT(13)
 * The value that changes is the duty cycle since that's how these motors work
 */

#define RIGHT		5 
#define LEFT		6
#define BAUDRATE	115200

byte rdata = 0, sel = 0;

void setup()
{
	//define as output
	DDRD |= _BV(RIGHT);
	DDRD |= _BV(LEFT);

	//enable pwm at 32kHz
	TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00); 
	TCCR0B = _BV(CS00); 

	//enable serial port comm
	Serial.begin(BAUDRATE);

	//initialize to off 
	analogWrite(LEFT, 0);
	analogWrite(RIGHT, 0);
}

void loop()
{
	//something is available
	if(Serial.available() > 0)
	{
		rdata = Serial.read();

		//send back an ACK
		Serial.write(rdata);

		//write dc
		analogWrite(RIGHT, byte(rdata));
		analogWrite(LEFT, byte(rdata));
	}
}

