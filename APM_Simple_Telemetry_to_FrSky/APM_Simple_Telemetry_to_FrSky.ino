#include <SoftwareSerial.h>
#include <FlexiTimer2.h>
#include "parser.h"
#include "FrSky.h"

#define HEARTBEATLED 13
#define HEARTBEATFREQ 500

#define DEBUG
#define DEBUGFRSKY

parser *p;
FrSky *frSky;
SoftwareSerial *debugSerial;
SoftwareSerial *frSkySerial;

long hbMillis;
byte hbState;
bool firstParse = false;

void setup() {
	// Debug serial port pin 11 tx, 12 rx
	debugSerial = new SoftwareSerial(12, 11);
	debugSerial->begin(38400);
	// FrSky data port pin 6 tx, 5 rx
#ifdef DEBUGFRSKY
	frSkySerial = new SoftwareSerial(6, 5);
#elif
	frSkySerial = new SoftwareSerial(6, 5, true);
#endif
	frSkySerial->begin(9600);
	// Incoming data from APM
	Serial.begin(57600);
	Serial.flush();
	
#ifdef DEBUG
	debugSerial->println("Initializing...");
#endif
	
	p = new parser(debugSerial);
	frSky = new FrSky(frSkySerial, p);
        
	digitalWrite(HEARTBEATLED, HIGH);
	hbState = HIGH;

#ifdef DEBUG
	debugSerial->println("Setting up timer.");
#endif

	FlexiTimer2::set(200, 1.0/1000, sendFrSkyData); // call every 200 1ms "ticks"
	FlexiTimer2::start();

#ifdef DEBUG
	debugSerial->println("Waiting for APM to boot.");
#endif

	// Blink fast a couple of times to wait for the APM to boot
	for (int i = 0; i < 100; i++)
	{
		if (i % 2)
		{
			digitalWrite(HEARTBEATLED, HIGH);
			hbState = HIGH;
		}
		else
		{
			digitalWrite(HEARTBEATLED, LOW);
			hbState = LOW;
		}
		delay(50);
	}

#ifdef DEBUG
	debugSerial->println("Initialization done.");
#endif
}

void loop() {
	// put your main code here, to run repeatedly: 
	readApmData();
	updateHeartbeat();
}

void updateHeartbeat()
{
	long currentMilillis = millis();
	if(currentMilillis - hbMillis > HEARTBEATFREQ) {
		hbMillis = currentMilillis;
		if (hbState == LOW)
		{
			hbState = HIGH;
		}
		else
		{
			hbState = LOW;
		}
		digitalWrite(HEARTBEATLED, hbState); 
	}
}

void sendFrSkyData()
{
	if (firstParse)
		frSky->sendFrSky();
}

void readApmData()
{  
	while (Serial.available() > 0)
	{
		//debugSerial->println("bla bla");
		debugSerial->println(Serial.available());
		//char temp = Serial.read();
		// Parse the motherfucker 
		bool done = p->parse(Serial.read());
		
		if (done && !firstParse)
		{
			firstParse = true;
#ifdef DEBUG
			debugSerial->println("First parse done. Start sending on frSky port.");
#endif
		}
#ifdef DEBUG
		if (done)
			frSky->printValues(debugSerial);
#endif
	}
}










