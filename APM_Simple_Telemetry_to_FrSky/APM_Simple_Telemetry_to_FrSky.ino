/*
	@author 	Nils Högberg
	@contact 	nils.hogberg@gmail.com

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 

#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];})) 

#include <SoftwareSerial.h>
#include <FlexiTimer2.h>
#include <FastSerial.h>
#include "SimpleTelemetry.h"
#include "Mavlink.h"
#include "FrSky.h"
#include "SimpleFIFO.h"
#include <GCS_MAVLink.h>
#define HEARTBEATLED 13
#define HEARTBEATFREQ 500


// Do not enable both at the same time
//#define DEBUG
//#define DEBUGFRSKY

// Comment this to run simple telemetry protocl
#define MAVLINKTELEMETRY

#ifdef MAVLINKTELEMETRY
Mavlink *dataProvider;
#else
SimpleTelemetry *dataProvider;
#endif

FastSerialPort0(Serial);
FrSky *frSky;
SoftwareSerial *frSkySerial;

#ifdef DEBUG
SoftwareSerial *debugSerial;
#elif defined DEBUGFRSKY
SoftwareSerial *frskyDebugSerial;
#endif

SimpleFIFO<char, 128> queue;

int		counter = 0;
unsigned long	hbMillis = 0;
unsigned long	rateRequestTimer = 0;
byte	hbState;
bool	firstParse = false;

void setup() {

// Debug serial port pin 11 rx, 12 tx
#ifdef DEBUG
	debugSerial = new SoftwareSerial(12, 11);
	debugSerial->begin(38400);
#elif defined DEBUGFRSKY
	frskyDebugSerial = new SoftwareSerial(12, 11);
	frskyDebugSerial->begin(38400);
#endif
	
	// FrSky data port pin 6 rx, 5 tx
	frSkySerial = new SoftwareSerial(6, 5, true);
	frSkySerial->begin(9600);
	// Incoming data from APM
	Serial.begin(57600);
	Serial.flush();
	
#ifdef DEBUG
	debugSerial->println("Initializing...");
	debugSerial->print("Free ram: ");
	debugSerial->print(freeRam());
	debugSerial->println(" bytes");
#endif
#ifdef MAVLINKTELEMETRY
	dataProvider = new Mavlink(&Serial);
#else
	dataProvider = new SimpleTelemetry();
#endif
	
	
	frSky = new FrSky();

	digitalWrite(HEARTBEATLED, HIGH);
	hbState = HIGH;

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
	debugSerial->print("Free ram: ");
	debugSerial->print(freeRam());
	debugSerial->println(" bytes");
#endif

}

void loop() {

#ifdef MAVLINKTELEMETRY
	if( dataProvider->enable_mav_request || (millis() - dataProvider->lastMAVBeat > 5000) )
	{
		if(millis() - rateRequestTimer > 2000)
		{
			for(int n = 0; n < 3; n++)
			{
#ifdef DEBUG
				debugSerial->println("Making rate request.");
#endif
				dataProvider->makeRateRequest();
				delay(50);
			}
			
			dataProvider->enable_mav_request = 0;
			dataProvider->waitingMAVBeats = 0;
			rateRequestTimer = millis();
		}
		
	}
#endif

	while (Serial.available() > 0)
	{
		if (queue.count() < 128)
		{
			char c = Serial.read();
			queue.enqueue(c);
		}
		else
		{
#ifdef DEBUG
			debugSerial->println("QUEUE IS FULL!");
#endif
		}
	}
	
	processData();
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
	counter++;
	
	if (counter >= 25)			 // Send 5000 ms frame
	{
		frSky->sendFrSky05Hz(frSkySerial, dataProvider);
		counter = 0;
	}
	else if ((counter % 5) == 0) // Send 1000 ms frame
	{
		frSky->sendFrSky1Hz(frSkySerial, dataProvider);
#ifdef DEBUG
		frSky->printValues(debugSerial, dataProvider);
#endif
	}
	else						 // Send 200 ms frame
	{
		frSky->sendFrSky5Hz(frSkySerial, dataProvider);
	}	
}

void processData()
{  
	while (queue.count() > 0)
	{ 
		bool done = dataProvider->parseMessage(queue.dequeue());

		if (done && !firstParse)
		{
			firstParse = true;
#ifdef DEBUG
			debugSerial->println("First parse done. Start sending on frSky port.");
#endif
		}
	}
}


int freeRam () {
	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}







