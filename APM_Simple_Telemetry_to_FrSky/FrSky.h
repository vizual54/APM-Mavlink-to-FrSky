#ifndef frsky_h
#define frsky_h

//#define lowByte(w) ((uint8_t) ((w) & 0xff))
//#define highByte(w) ((uint8_t) ((w) >> 8))

#include "parser.h"
#include <SoftwareSerial.h>
#include "defines.h"

// The following data is sent from APM
// 0  $ - Header
// 1  Main battery voltage in V              
// 2  Battery current in mA                  
// 3  Battery remaining in %                 
// 4  GPS Status 0:No Fix, 2:2D Fix, 3:3D Fix
// 5  GPS Latitude in decimal degrees        
// 6  GPS Longitude in decimal degrees       
// 7  GPS Altitude in cm                     
// 8  GPS hdop                               
// 9  GPS Number of satelites in view        
// 10 GPS Ground speed in cm/s               
// 11 Course in 1/100 degree                 
// 12 Distance to arm position in cm         
// 13 Altitude in cm         
// 14 Home altitude in cm    
// 15 APM mode                               
// 16 Compass                                
// 17 Throttle out           
// 18 Accel-X
// 19 Accel-Y
// 20 Accel-Z
// * - end

class FrSky
{
	
public:
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
		parser			*par;
		unsigned char	frskyBuffer[48];
		int				bufferLength;
		
		float gpsDdmToDmdmFormat(float ddm);
		byte lsByte(int value);
		byte msByte(int value);
};

#endif
