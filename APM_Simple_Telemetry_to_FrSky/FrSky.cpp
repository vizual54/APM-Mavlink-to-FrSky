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

#include "FrSky.h"

//#define DEBUGFRSKY

FrSky::FrSky(SoftwareSerial* serialPort, SoftwareSerial* debugSerialPort, parser* p)
{
	frSkySerial = serialPort;
	debugPort = debugSerialPort;
	par = p;

	bufferLength = 0;
}

FrSky::FrSky(SoftwareSerial* serialPort, parser* p)
{
	frSkySerial = serialPort;
	par = p;

	bufferLength = 0;
}

FrSky::~FrSky(void)
{
}

// We receive the GPS coordinates in ddd.dddd format
// FrSky wants the dd mm.mmm format so convert.
float FrSky::gpsDdToDmsFormat(float ddm)
{
	int deg = (int)ddm;
	float min_dec = (ddm - deg) * 60.0f;
	float sec = (min_dec - (int)min_dec) * 60.0f;
	/*
	float dec_mm = (ddm - (int)ddm) * 60.0f;
	int mm = (int)dec_mm;
	int ss = (int)((dec_mm - mm) * 60.0f);
	return ((int)ddm * 100.0f) + mm + ss / 100.0f;
	*/
	return (float)deg * 100.0f + (int)min_dec + sec / 100.0f;
}

void FrSky::sendFrSky05Hz()
{
	// Date, Time
	/*
	bufferLength += addBufferData(DATE);
	bufferLength += addBufferData(TIME);
	frskyBuffer[bufferLength++] = tail_value;
	bufferLength = writeBuffer(bufferLength);
	*/
}

// Send 1000 ms frame
void FrSky::sendFrSky1Hz()
{
	
	// Course, Latitude, Longitude, Speed, Altitude (GPS), Fuel Level
	bufferLength += addBufferData(COURSE);
	bufferLength += addBufferData(LATITUDE);
	bufferLength += addBufferData(LONGITUDE);
	bufferLength += addBufferData(GPSSPEED);
	bufferLength += addBufferData(GPSALT);
	bufferLength += addBufferData(FUEL);
	frskyBuffer[bufferLength++] = tail_value;
	bufferLength = writeBuffer(bufferLength);
	
}

// Send 200 ms frame
void FrSky::sendFrSky5Hz()
{
	
	// Three-axis Acceleration Values, Altitude (variometer-0.01m), Tempature1, Temprature2, Voltage , Current & Voltage (Ampere Sensor) , RPM
	bufferLength += addBufferData(ACCX);
	bufferLength += addBufferData(ACCY);
	bufferLength += addBufferData(ACCZ);
	bufferLength += addBufferData(ALTITUDE);
	bufferLength += addBufferData(TEMP1);
	bufferLength += addBufferData(TEMP2);
	bufferLength += addBufferData(INDVOLT);
	bufferLength += addBufferData(CURRENT);
	bufferLength += addBufferData(VOLTAGE);
	bufferLength += addBufferData(RPM);
	frskyBuffer[bufferLength++] = tail_value;
	bufferLength = writeBuffer(bufferLength);
	
}

byte FrSky::lsByte(int value)
{
  return ((byte) ((value) & 0xff));
}

byte FrSky::msByte(int value)
{
  return ((byte) ((value) >> 8));
}

unsigned char FrSky::addBufferData(const char id)
{

	switch(id) {
		case GPSALT :
		{
			float gpsAltitude = par->termToDecimal(6) * 100.0f + 100.0f; // GPS Altitude i m, offset by 100

			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = GPSALT;
			frskyBuffer[bufferLength + 2] = lsByte((int)gpsAltitude);
			frskyBuffer[bufferLength + 3] = msByte((int)gpsAltitude);
      
			unsigned int temp = (unsigned int)((gpsAltitude - (int)gpsAltitude) * 1000.0f);

			frskyBuffer[bufferLength + 4] = header_value;
			frskyBuffer[bufferLength + 5] = GPSALT + decimal;
			frskyBuffer[bufferLength + 6] = lsByte(temp);
			frskyBuffer[bufferLength + 7] = msByte(temp);
      
			return 8;
			break;
		}
		case TEMP1 :
		{
			// APM mode
			int temp1 = (int)par->termToDecimal(13);
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = TEMP1;
			frskyBuffer[bufferLength + 2] = lsByte(temp1);
			frskyBuffer[bufferLength + 3] = msByte(temp1);
      
			return 4;
			break;
		}
		case RPM :
		{
			// Throttle out
			int engineSpeed = (int)par->termToDecimal(15) / 30;
			
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = RPM;
			frskyBuffer[bufferLength + 2] = lsByte(engineSpeed);
			frskyBuffer[bufferLength + 3] = msByte(engineSpeed);
			return 4;
			break;
		}
		case FUEL :
		{
			// Battery remaining in %
			int fuelLevel = (int)par->termToDecimal(2); 
			
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = FUEL;
			frskyBuffer[bufferLength + 2] = lsByte(fuelLevel);
			frskyBuffer[bufferLength + 3] = msByte(fuelLevel); 
			
			return 4;
			break;
		}
		case TEMP2 :
		{
			// GPS status mode, number of satelites in view
			int nrSats = (int)par->termToDecimal(8);    // GPS Number of satelites in view 
			int gpsStatus = (int)par->termToDecimal(3); // GPS Status 0:No Fix, 2:2D Fix, 3:3D Fix
			int value = gpsStatus * 10 + nrSats;
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = TEMP2;
			frskyBuffer[bufferLength + 2] = lsByte(value);
			frskyBuffer[bufferLength + 3] = msByte(value);
			return 4;
			break;
		}
		case INDVOLT :
		{
			return 0;
			break;
		}
		case ALTITUDE :
		{
			// Altitude in cm minus Home altitude in cm
			float altitude = ((par->termToDecimal(11) - par->termToDecimal(12)) / 100.0f) + 10.0f; // Altitude in Taranis is offset by -10 m
			
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = ALTITUDE;
			frskyBuffer[bufferLength + 2] = lsByte((int)altitude);
			frskyBuffer[bufferLength + 3] = msByte((int)altitude);
      
			unsigned int temp = (unsigned int)((altitude - (int)altitude) * 100.0f);
    
			frskyBuffer[bufferLength + 4] = header_value;
			frskyBuffer[bufferLength + 5] = ALTIDEC;
			frskyBuffer[bufferLength + 6] = lsByte(temp);
			frskyBuffer[bufferLength + 7] = msByte(temp);
			return 8;
			break;
		}
		case GPSSPEED :
		{
			// GPS Ground speed in knots
			float gpsSpeed = par->termToDecimal(9) * 0.0194384f / 1.84f; // Seems like there is an offset of 1.84 for some reason

			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = GPSSPEED;
			frskyBuffer[bufferLength + 2] = lsByte((int)gpsSpeed);
			frskyBuffer[bufferLength + 3] = msByte((int)gpsSpeed);
      
			unsigned int temp = (unsigned int)((gpsSpeed - (int)gpsSpeed) * 1000.0f);
    
			frskyBuffer[bufferLength + 4] = header_value;
			frskyBuffer[bufferLength + 5] = GPSSPEED + decimal;
			frskyBuffer[bufferLength + 6] = lsByte(temp);
			frskyBuffer[bufferLength + 7] = msByte(temp);
      
			return 8;
			break;
		}
		case LATITUDE :
		{
			float gpsLatitude = gpsDdToDmsFormat(par->termToDecimal(4) / 10000000.0f);

			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = LATITUDE;
			frskyBuffer[bufferLength + 2] = lsByte((int)gpsLatitude);
			frskyBuffer[bufferLength + 3] = msByte((int)gpsLatitude);
      
			unsigned int temp = (unsigned int)((gpsLatitude - (int)gpsLatitude) * 10000.0f);
    
			frskyBuffer[bufferLength + 4] = header_value;
			frskyBuffer[bufferLength + 5] = LATITUDE + decimal;
			frskyBuffer[bufferLength + 6] = lsByte(temp);
			frskyBuffer[bufferLength + 7] = msByte(temp);
      
			char northSouth = (gpsLatitude < 0) ? 'S' : 'N';
			frskyBuffer[bufferLength + 8] = header_value;
			frskyBuffer[bufferLength + 9] = NORTHSOUTH;
			frskyBuffer[bufferLength + 10] = northSouth;
			frskyBuffer[bufferLength + 11] = 0;
      
			return 12;
			break;
		}
		case LONGITUDE :
		{
			float gpsLongitude = gpsDdToDmsFormat(par->termToDecimal(5) / 10000000.0f);

			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = LONGITUDE;
			frskyBuffer[bufferLength + 2] = lsByte((int)gpsLongitude);
			frskyBuffer[bufferLength + 3] = msByte((int)gpsLongitude);
      
			unsigned int temp = (unsigned int)((gpsLongitude - (int)gpsLongitude) * 10000.0f);
    
			frskyBuffer[bufferLength + 4] = header_value;
			frskyBuffer[bufferLength + 5] = LONGITUDE + decimal;
			frskyBuffer[bufferLength + 6] = lsByte(temp);
			frskyBuffer[bufferLength + 7] = msByte(temp);
			
			char eastWest = (gpsLongitude < 0 ) ? 'W' : 'E';
			frskyBuffer[bufferLength + 8] = header_value;
			frskyBuffer[bufferLength + 9] = EASTWEST;
			frskyBuffer[bufferLength + 10] = eastWest;
			frskyBuffer[bufferLength + 11] = 0;
      
			return 12;
			break;
		}
		case COURSE :
		{
			//float course = (par->termToDecimal(14) / 100.0f); // Course in 1/100 degree
			float course = 270.0f;
			
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = COURSE;
			frskyBuffer[bufferLength + 2] = lsByte((int)course);
			frskyBuffer[bufferLength + 3] = msByte((int)course);
      
			unsigned int temp = (unsigned int)((course - (int)course) * 1000.0f);
    
			frskyBuffer[bufferLength + 4] = header_value;
			frskyBuffer[bufferLength + 5] = COURSE + decimal;
			frskyBuffer[bufferLength + 6] = lsByte(temp);
			frskyBuffer[bufferLength + 7] = msByte(temp);
      
			return 8;
			break;
		}
		case DATE :
		{
			float date = 0.0f;
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = DATE;
			frskyBuffer[bufferLength + 2] = lsByte((int)date);
			frskyBuffer[bufferLength + 3] = msByte((int)date);
			return 4;
			break;
		}
		case YEAR :
		{
			return 0;
			break;
		}
		case TIME :
		{
			float time = 0.0f;
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = TIME;
			frskyBuffer[bufferLength + 2] = lsByte((int)time);
			frskyBuffer[bufferLength + 3] = msByte((int)time);
			return 4;
			break;
		}
		case SECOND :
		{
			return 0;
			break;
		}
		case ACCX :
		{
			float accX = par->termToDecimal(17) / 100.0f;

			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = ACCX;
			frskyBuffer[bufferLength + 2] = lsByte((int)(accX*1000.0f));
			frskyBuffer[bufferLength + 3] = msByte((int)(accX*1000.0f));
			return 4;
			break;
		}
		case ACCY :
		{
			float accY = par->termToDecimal(18) / 100.0f;
			
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = ACCY;
			frskyBuffer[bufferLength + 2] = lsByte((int)(accY*1000.0f));
			frskyBuffer[bufferLength + 3] = msByte((int)(accY*1000.0f));
			return 4;
			break;
		}
		case ACCZ :
		{
			float accZ = par->termToDecimal(19) / 100.0f;

			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = ACCZ;
			frskyBuffer[bufferLength + 2] = lsByte((int)(accZ*1000.0f));
			frskyBuffer[bufferLength + 3] = msByte((int)(accZ*1000.0f));
			return 4;
			break;
		}
		case CURRENT :
		{
			float current = par->termToDecimal(1) / 1000.0f; // 10.0f -> 1A
			
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = CURRENT;
			frskyBuffer[bufferLength + 2] = lsByte((int)(current));
			frskyBuffer[bufferLength + 3] = msByte((int)(current));
			return 4;
			break;
		}
		case VOLTAGE :
		{
			float batteryVoltage = par->termToDecimal(0) * 0.5238f;
			//float batteryVoltage = 100.0f * 0.5238;
			
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = VOLTAGE;
			frskyBuffer[bufferLength + 2] = lsByte((int)batteryVoltage);
			frskyBuffer[bufferLength + 3] = msByte((int)batteryVoltage);
      
			unsigned int temp = (unsigned int)((batteryVoltage - (int)batteryVoltage) * 10.0f);
    
			frskyBuffer[bufferLength + 4] = header_value;
			frskyBuffer[bufferLength + 5] = VOLTAGEDEC;
			frskyBuffer[bufferLength + 6] = lsByte(temp);
			frskyBuffer[bufferLength + 7] = msByte(temp);

			return 8;
			break;
		}
		default :
			return 0;
  }
  return 0;
}

unsigned char FrSky::writeBuffer(const int length)
{

  int i = 0;
  while(i < length)
  {
    // If a data value is equal to header (0x5E), tail (0x5E) or escape (0x5D) value exchange it.
    // There is always a id and two bytes between header and tail therefor every 4th byte is a header and should not be checked
    if ((i % 4))
    {
      switch (frskyBuffer[i])
      {
        case header_value :
        {
          frSkySerial->write((byte)0x5D);
          frSkySerial->write((byte)0x3E);
#ifdef DEBUGFRSKY
		  debugPort->write((byte)0x5D);
		  debugPort->write((byte)0x3E);
#endif
          break;
        }
        case escape_value :
        {
		  frSkySerial->write((byte)0x5D);
          frSkySerial->write((byte)0x3D);
#ifdef DEBUGFRSKY
		  debugPort->write((byte)0x5D);
          debugPort->write((byte)0x3D);
#endif
          break;
        }
        default :
        {
          frSkySerial->write((byte)frskyBuffer[i]);
#ifdef DEBUGFRSKY
		  debugPort->write((byte)frskyBuffer[i]);
#endif
        }
      }
    }
    else
    {
      frSkySerial->write((byte)frskyBuffer[i]);
#ifdef DEBUGFRSKY
	  debugPort->write((byte)frskyBuffer[i]);
#endif
    }
    
    i++;
  }
  
  return 0;
}

void FrSky::printValues(SoftwareSerial* debugPort)
{
	
	debugPort->print("Voltage: ");
	debugPort->print(par->termToDecimal(0), 2);
	debugPort->print(" Current: ");
	debugPort->print(par->termToDecimal(1) / 100.0f, 2);
	debugPort->print(" Fuel: ");
	debugPort->print(par->termToDecimal(2), 2);
	debugPort->print(" GPS status: ");
	debugPort->print((int)par->termToDecimal(3));
	debugPort->print(" Latitude: ");
	debugPort->print(gpsDdToDmsFormat(par->termToDecimal(4) / 10000000.0f), 2);
	debugPort->print(" Longitude: ");
	debugPort->print(gpsDdToDmsFormat(par->termToDecimal(5) / 10000000.0f), 2);
	debugPort->print(" GPS Alt: ");
	debugPort->print(par->termToDecimal(6) / 100.0f, 2);
	debugPort->print(" GPS hdop: ");
	debugPort->print(par->termToDecimal(7) / 100.0f, 2);
	debugPort->print(" GPS sats: ");
	debugPort->print((int)par->termToDecimal(8));
	debugPort->print(" GPS speed: ");
	debugPort->print(par->termToDecimal(9) * 0.0194384f, 2);
	debugPort->print(" GPS Course: ");
	debugPort->print(par->termToDecimal(10) / 100.0f, 2);
	debugPort->print(" Home alt: ");
	debugPort->print((par->termToDecimal(11) - par->termToDecimal(12)) / 100.0f, 2);
	debugPort->print(" Mode: ");
	debugPort->print((int)par->termToDecimal(13));
	debugPort->print(" Course: ");
	debugPort->print(par->termToDecimal(14) / 100.0f, 2);
	debugPort->print(" RPM: ");
	debugPort->print((int)par->termToDecimal(15));
	debugPort->println("");
}
