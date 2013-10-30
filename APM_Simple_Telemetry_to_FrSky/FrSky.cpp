#include "FrSky.h"

FrSky::FrSky(SoftwareSerial* serialPort, parser* p)
{
	frSkySerial = serialPort;
	par = p;

	bufferLength = 0;
}


FrSky::~FrSky(void)
{
}

void FrSky::saveValues()
{
	// Save the values.
      
      //gpsAltitude = par->termToDecimal(7);
      //fuelLevel = (int)par->termToDecimal(3);
      //altitude = par->termToDecimal(14);
      //gpsSpeed = par->termToDecimal(10);
      //gpsLongitude = par->termToDecimal(13);
      //gpsLatitude = par->termToDecimal(12);
      //current = par->termToDecimal(2);
      //batteryVoltage = par->termToDecimal(1);
}

// We receive the GPS coordinates in ddd.dddd format
// FrSky wants the dd mm.mmm format so convert.
float FrSky::gpsDdToDmsFormat(float ddm)
{
	//int dd = (int)ddm;

	float dec_mm = (ddm - (int)ddm) * 60.0f;
	int mm = (int)dec_mm;
	int ss = (int)((dec_mm - mm) * 60.0f);
	return ((int)ddm * 100.0f) + mm + ss / 100.0f;
}

void FrSky::sendFrSky05Hz()
{
	// Date, Time
	/*
	bufferLength += addBufferData(DATE);
	bufferLength += addBufferData(TIME);
	frskyBuffer[++bufferLength] = tail_value;
	bufferLength = writeBuffer(bufferLength);
	*/
}

// Send 1000 ms frame
void FrSky::sendFrSky1Hz()
{
	// Course, Latitude, Longitude, Speed, Altitude (GPS), Fuel Level
	/*
	bufferLength += addBufferData(COURSE);
	bufferLength += addBufferData(LATITUDE);
	bufferLength += addBufferData(LONGITUDE);
	bufferLength += addBufferData(GPSSPEED);
	bufferLength += addBufferData(GPSALT);
	bufferLength += addBufferData(FUEL);
	frskyBuffer[++bufferLength] = tail_value;
	bufferLength = writeBuffer(bufferLength);
	*/
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
			//float gpsAltitude = par->termToDecimal(6) * 100.0f; // GPS Altitude i m
			float gpsAltitude = 0.0f;

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
			int engineSpeed = (int)par->termToDecimal(15);
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
			if (fuelLevel <= 40)
			{
				frskyBuffer[bufferLength + 2] = lsByte(25);
				frskyBuffer[bufferLength + 3] = msByte(25);      
			}
			else if (fuelLevel > 40 && fuelLevel <= 60)
			{
				frskyBuffer[bufferLength + 2] = lsByte(50);
				frskyBuffer[bufferLength + 3] = msByte(50);
			}
			else if (fuelLevel > 60 && fuelLevel <= 80)
			{
				frskyBuffer[bufferLength + 2] = lsByte(75);
				frskyBuffer[bufferLength + 3] = msByte(75);
			}
			else
			{
				frskyBuffer[bufferLength + 2] = lsByte(100);
				frskyBuffer[bufferLength + 3] = msByte(100);
			}
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
			//float altitude = (par->termToDecimal(11) - par->termToDecimal(12)) / 100.0f;
			float altitude = 0.0f;
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = ALTITUDE;
			frskyBuffer[bufferLength + 2] = lsByte((int)altitude);
			frskyBuffer[bufferLength + 3] = msByte((int)altitude);
      
			unsigned int temp = (unsigned int)((altitude - (int)altitude) * 1000.0f);
    
			frskyBuffer[bufferLength + 4] = header_value;
			frskyBuffer[bufferLength + 5] = ALTIDEC;
			frskyBuffer[bufferLength + 6] = lsByte(temp);
			frskyBuffer[bufferLength + 7] = msByte(temp);
			return 8;
			break;
		}
		case GPSSPEED :
		{
			//float gpsSpeed = par->termToDecimal(9) * 0.0194384f; // GPS Ground speed in knots
			float gpsSpeed = 0.0f;

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
      
			unsigned int temp = (unsigned int)((gpsLatitude - (int)gpsLatitude) * 1000.0f);
    
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
      
			unsigned int temp = (unsigned int)((gpsLongitude - (int)gpsLongitude) * 1000.0f);
    
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
			float course = 0.0f;
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
			//float accX = par->termToDecimal(17) / 100.0f;
			float accX = 0.0f;
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = ACCX;
			frskyBuffer[bufferLength + 2] = lsByte((int)(accX*100.0f));
			frskyBuffer[bufferLength + 3] = msByte((int)(accX*100.0f));
			return 4;
			break;
		}
		case ACCY :
		{
			//float accY = par->termToDecimal(18) / 100.0f;
			float accY = 0.0f;
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = ACCY;
			frskyBuffer[bufferLength + 2] = lsByte((int)(accY*100.0f));
			frskyBuffer[bufferLength + 3] = msByte((int)(accY*100.0f));
			return 4;
			break;
		}
		case ACCZ :
		{
			//float accZ = par->termToDecimal(19) / 100.0f;
			float accZ = 0.0f;
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = ACCZ;
			frskyBuffer[bufferLength + 2] = lsByte((int)(accZ*100.0f));
			frskyBuffer[bufferLength + 3] = msByte((int)(accZ*100.0f));
			return 4;
			break;
		}
		case CURRENT :
		{
			//float current = par->termToDecimal(1);
			float current = 0.0f;
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = CURRENT;
			frskyBuffer[bufferLength + 2] = lsByte((int)(current*100.0f));
			frskyBuffer[bufferLength + 3] = msByte((int)(current*100.0f));
			return 4;
			break;
		}
		case VOLTAGE :
		{
			//float batteryVoltage = par->termToDecimal(0);
			float batteryVoltage = 0.0f;
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = VOLTAGE;
			frskyBuffer[bufferLength + 2] = lsByte((int)batteryVoltage);
			frskyBuffer[bufferLength + 3] = msByte((int)batteryVoltage);
      
			unsigned int temp = (unsigned int)((batteryVoltage - (int)batteryVoltage) * 1000.0f);
    
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
	debugPort->println("");
}
