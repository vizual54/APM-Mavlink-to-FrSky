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

#ifndef frsky_h
#define frsky_h

//#define lowByte(w) ((uint8_t) ((w) & 0xff))
//#define highByte(w) ((uint8_t) ((w) >> 8))

#include "parser.h"
#include <SoftwareSerial.h>
#include "defines.h"

// The following data is sent from APM
//   $ - Header
// 0  Main battery voltage in V              
// 1  Battery current in mA                  
// 2  Battery remaining in %                 
// 3  GPS Status 0:No Fix, 2:2D Fix, 3:3D Fix
// 4  GPS Latitude in decimal degrees        
// 5  GPS Longitude in decimal degrees       
// 6  GPS Altitude in cm                     
// 7  GPS hdop                               
// 8  GPS Number of satelites in view        
// 9  GPS Ground speed in cm/s               
// 10 GPS Course in 1/100 degree                 
// 11 Altitude in cm         
// 12 Home altitude in cm    
// 13 APM mode                               
// 14 Compass                                
// 15 Throttle out           
// 16 Accel-X
// 17 Accel-Y
// 18 Accel-Z
// * - end

class FrSky
{
	
public:
		FrSky(SoftwareSerial* serialPort, SoftwareSerial* debugSerialPort, parser* p);
		FrSky(SoftwareSerial* serialPort, parser* p);
		~FrSky(void);
		void saveValues();
		void sendFrSky5Hz();
		void sendFrSky1Hz();
		void sendFrSky05Hz();
		unsigned char addBufferData(const char id);
		unsigned char writeBuffer(const int length);
		void printValues(SoftwareSerial* debugPort);
		
private:
		SoftwareSerial	*frSkySerial;
		SoftwareSerial	*debugPort;
		parser			*par;
		unsigned char	frskyBuffer[64];
		int				bufferLength;
		long FixInt(long val, byte mp);
		float gpsDdToDmsFormat(float ddm);
		byte lsByte(int value);
		byte msByte(int value);
};

#endif
