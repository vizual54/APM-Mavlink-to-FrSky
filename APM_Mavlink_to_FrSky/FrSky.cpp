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

FrSky::FrSky()
{
	bufferLength = 0;
}
FrSky::~FrSky(void)
{
}

void FrSky::sendFrSky05Hz(SoftwareSerial* serialPort, IFrSkyDataProvider* dataProvider)
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
void FrSky::sendFrSky1Hz(SoftwareSerial* serialPort, IFrSkyDataProvider* dataProvider)
{
	
	// Course, Latitude, Longitude, Speed, Altitude (GPS), Fuel Level
	bufferLength += addBufferData(COURSE, dataProvider);
	bufferLength += addBufferData(LATITUDE,dataProvider);
	bufferLength += addBufferData(LONGITUDE, dataProvider);
	bufferLength += addBufferData(GPSSPEED, dataProvider);
	bufferLength += addBufferData(GPSALT,dataProvider);
	bufferLength += addBufferData(FUEL, dataProvider);
	frskyBuffer[bufferLength++] = tail_value;
	bufferLength = writeBuffer(bufferLength, serialPort);
	
}

// Send 200 ms frame
void FrSky::sendFrSky5Hz(SoftwareSerial* serialPort, IFrSkyDataProvider* dataProvider)
{
	
	// Three-axis Acceleration Values, Altitude (variometer-0.01m), Tempature1, Temprature2, Voltage , Current & Voltage (Ampere Sensor) , RPM
	bufferLength += addBufferData(ACCX,dataProvider);
	bufferLength += addBufferData(ACCY, dataProvider);
	bufferLength += addBufferData(ACCZ, dataProvider);
	bufferLength += addBufferData(ALTITUDE, dataProvider);
	bufferLength += addBufferData(TEMP1, dataProvider);
	bufferLength += addBufferData(TEMP2, dataProvider);
	bufferLength += addBufferData(INDVOLT, dataProvider);
	bufferLength += addBufferData(CURRENT, dataProvider);
	bufferLength += addBufferData(VOLTAGE, dataProvider);
	bufferLength += addBufferData(RPM, dataProvider);
	frskyBuffer[bufferLength++] = tail_value;
	bufferLength = writeBuffer(bufferLength, serialPort);
	
}

byte FrSky::lsByte(int value)
{
  return ((byte) ((value) & 0xff));
}

byte FrSky::msByte(int value)
{
  return ((byte) ((value) >> 8));
}

unsigned char FrSky::addBufferData(const char id, IFrSkyDataProvider* dataProvider)
{

	switch(id) {
		case GPSALT :
		{
			//float gpsAltitude = par->termToDecimal(6) * 100.0f + 100.0f; // GPS Altitude i m, offset by 100
			float gpsAltitude = dataProvider->getGpsAltitude() + 100.0f;
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
			int temp1 = dataProvider->getTemp1();
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
			int engineSpeed = dataProvider->getEngineSpeed() / 30;
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
			int fuelLevel = dataProvider->getFuelLevel();
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
			int value = dataProvider->getTemp2();
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
			// Altitude in Taranis is offset by -10 m
			float altitude = dataProvider->getAltitude() + 10.0f;
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
			// Seems like there is an offset of 1.84 for some reason
			float gpsSpeed  = dataProvider->getGpsGroundSpeed() / 1.84f;
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
			//float gpsLatitude = gpsDdToDmsFormat(termToDecimal(4) / 10000000.0f);
			float gpsLatitude = dataProvider->getLatitude();
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
			//float gpsLongitude = gpsDdToDmsFormat(termToDecimal(5) / 10000000.0f);
			float gpsLongitude = dataProvider->getLongitud();
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
			float course = dataProvider->getCourse();
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
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = DATE;
			frskyBuffer[bufferLength + 2] = lsByte(dataProvider->getDate());
			frskyBuffer[bufferLength + 3] = msByte(dataProvider->getDate());
			return 4;
			break;
		}
		case YEAR :
		{
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = DATE;
			frskyBuffer[bufferLength + 2] = lsByte(dataProvider->getYear());
			frskyBuffer[bufferLength + 3] = msByte(dataProvider->getYear());
			return 4;
			break;
		}
		case TIME :
		{
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = TIME;
			frskyBuffer[bufferLength + 2] = lsByte(dataProvider->getTime());
			frskyBuffer[bufferLength + 3] = msByte(dataProvider->getTime());
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
			//float accX = par->termToDecimal(17) / 100.0f;
			float accX = dataProvider->getAccX();
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = ACCX;
			frskyBuffer[bufferLength + 2] = lsByte((int)(accX*1000.0f));
			frskyBuffer[bufferLength + 3] = msByte((int)(accX*1000.0f));
			return 4;
			break;
		}
		case ACCY :
		{
			//float accY = par->termToDecimal(18) / 100.0f;
			float accY =  dataProvider->getAccY();
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = ACCY;
			frskyBuffer[bufferLength + 2] = lsByte((int)(accY*1000.0f));
			frskyBuffer[bufferLength + 3] = msByte((int)(accY*1000.0f));
			return 4;
			break;
		}
		case ACCZ :
		{
			//float accZ = par->termToDecimal(19) / 100.0f;
			float accZ = dataProvider->getAccZ();
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = ACCZ;
			frskyBuffer[bufferLength + 2] = lsByte((int)(accZ*1000.0f));
			frskyBuffer[bufferLength + 3] = msByte((int)(accZ*1000.0f));
			return 4;
			break;
		}
		case CURRENT :
		{
			//float current = par->termToDecimal(1) / 1000.0f; // 10.0f -> 1A
			float current = dataProvider->getBatteryCurrent();
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = CURRENT;
			frskyBuffer[bufferLength + 2] = lsByte((int)(current));
			frskyBuffer[bufferLength + 3] = msByte((int)(current));
			return 4;
			break;
		}
		case VOLTAGE :
		{
			//float batteryVoltage = par->termToDecimal(0) * 0.5238f;
			//float batteryVoltage = 100.0f * 0.5238;
			float batteryVoltage = dataProvider->getMainBatteryVoltage() * 0.5238f;
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

unsigned char FrSky::writeBuffer(const int length, SoftwareSerial* frSkySerial)
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
          break;
        }
        case escape_value :
        {
		  frSkySerial->write((byte)0x5D);
          frSkySerial->write((byte)0x3D);
          break;
        }
        default :
        {
          frSkySerial->write((byte)frskyBuffer[i]);
        }
      }
    }
    else
    {
      frSkySerial->write((byte)frskyBuffer[i]);
    }
    
    i++;
  }
  
  return 0;
}

void FrSky::printValues(SoftwareSerial* serialPort, IFrSkyDataProvider* dataProvider)
{
	serialPort->print("Voltage: ");
	serialPort->print(dataProvider->getMainBatteryVoltage(), 2);
	serialPort->print(" Current: ");
	serialPort->print(dataProvider->getBatteryCurrent(), 2);
	serialPort->print(" Fuel: ");
	serialPort->print(dataProvider->getFuelLevel());
	serialPort->print(" Latitude: ");
	serialPort->print(dataProvider->getLatitude(), 6);
	serialPort->print(" Longitude: ");
	serialPort->print(dataProvider->getLongitud(), 6);
	serialPort->print(" GPS Alt: ");
	serialPort->print(dataProvider->getGpsAltitude(), 2);
	//serialPort->print(" GPS hdop: ");
	//serialPort->print(dataProvider->getGpsHdop(), 2);
	serialPort->print(" GPS status + sats: ");
	serialPort->print(dataProvider->getTemp2());
	serialPort->print(" GPS speed: ");
	serialPort->print(dataProvider->getGpsGroundSpeed(), 2);
	serialPort->print(" Home alt: ");
	serialPort->print(dataProvider->getAltitude(), 2);
	serialPort->print(" Mode: ");
	serialPort->print(dataProvider->getTemp1());
	serialPort->print(" Course: ");
	serialPort->print(dataProvider->getCourse(), 2);
	serialPort->print(" RPM: ");
	serialPort->print(dataProvider->getEngineSpeed());
	serialPort->print(" AccX: ");
	serialPort->print(dataProvider->getAccX(), 2);
	serialPort->print(" AccY: ");
	serialPort->print(dataProvider->getAccY(), 2);
	serialPort->print(" AccZ: ");
	serialPort->print(dataProvider->getAccZ(), 2);
	serialPort->println("");
}
