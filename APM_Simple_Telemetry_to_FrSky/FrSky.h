#ifndef frsky_h
#define frsky_h

//#define lowByte(w) ((uint8_t) ((w) & 0xff))
//#define highByte(w) ((uint8_t) ((w) >> 8))

#include "parser.h"
#include <SoftwareSerial.h>
#include "defines.h"

// The following data is sent from APM
// 0  $ - Header
// 1  Main battery voltage in V                int16_t
// 2  Battery current in mA                    int16_t
// 3  Battery remaining in %                   int8_t
// 4  GPS Status 0:No Fix, 2:2D Fix, 3:3D Fix  uint8_t 
// 5  GPS Latitude in decimal degrees          int32_t
// 6  GPS Longitude in decimal degrees         int32_t
// 7  GPS Altitude in m                        int32_t
// 8  GPS hdop                                 int16_t
// 9  GPS Number of satelites in view          uint8_t
// 10 GPS Ground speed in cm/s                 uint32_t
// 11 Distance to arm position in m            int32_t
// 12 latitude in dec deg                      int32_t
// 13 longitude in dec deg                     int32_t
// 14 altitude in m                            int32_t
// 15 home altitude in m                       int32_t
// 16 APM mode                                 uint8_t
// * - end

class FrSky
{
	
public:
		FrSky(SoftwareSerial* serialPort, parser* p);
		~FrSky(void);
		void saveValues();
		void sendFrSky();
		unsigned char addBufferData(const char id);
		unsigned char writeBuffer(const int length);
		void printValues(SoftwareSerial* debugPort);
		
private:
		SoftwareSerial	*frSkySerial;
		parser			*par;
		unsigned char	frskyBuffer[48];
		int				bufferLength;
		int				counter;
		
		float gpsDdmToDmdmFormat(float ddm);
		byte lsByte(int value);
		byte msByte(int value);
};

#endif
