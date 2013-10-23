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

// We receive the GPS coordinated in ddd.dddd format
// FrSky wants the dd mm.mmm format so convert.
float FrSky::gpsDdmToDmdmFormat(float ddm)
{
	return 0.0f;
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
			float gpsAltitude = par->termToDecimal(7) * 1000.0f; //  Main battery voltage in V
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = GPSALT;
			frskyBuffer[bufferLength + 2] = msByte((int)gpsAltitude);
			frskyBuffer[bufferLength + 3] = lsByte((int)gpsAltitude);
      
			unsigned int temp = (unsigned int)((gpsAltitude - (int)gpsAltitude) * 1000.0f);

			frskyBuffer[bufferLength + 4] = header_value;
			frskyBuffer[bufferLength + 5] = GPSALT + decimal;
			frskyBuffer[bufferLength + 6] = msByte(temp);
			frskyBuffer[bufferLength + 7] = lsByte(temp);
      
			return 8;
			break;
		}
		case TEMP1 :
		{
			// APM mode
			float temp1 = par->termToDecimal(15); // 15 APM mode
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = TEMP1;
			frskyBuffer[bufferLength + 2] = msByte(temp1);
			frskyBuffer[bufferLength + 3] = lsByte(temp1);
      
			return 4;
			break;
		}
		case RPM :
		{
			float engineSpeed = par->termToDecimal(17); // Throttle out

			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = RPM;
			frskyBuffer[bufferLength + 2] = msByte(engineSpeed);
			frskyBuffer[bufferLength + 3] = lsByte(engineSpeed);
			return 4;
			break;
		}
		case FUEL :
		{
			float fuelLevel = par->termToDecimal(3); // Battery remaining in %
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = FUEL;
			if (fuelLevel <= 40)
			{
				frskyBuffer[bufferLength + 2] = msByte(25);
				frskyBuffer[bufferLength + 3] = lsByte(25);      
			}
			else if (fuelLevel > 40 && fuelLevel <= 60)
			{
				frskyBuffer[bufferLength + 2] = msByte(50);
				frskyBuffer[bufferLength + 3] = lsByte(50);
			}
			else if (fuelLevel > 60 && fuelLevel <= 80)
			{
				frskyBuffer[bufferLength + 2] = msByte(75);
				frskyBuffer[bufferLength + 3] = lsByte(75);
			}
			else
			{
				frskyBuffer[bufferLength + 2] = msByte(100);
				frskyBuffer[bufferLength + 3] = lsByte(100);
			}
			return 4;
			break;
		}
		case TEMP2 :
		{
			// GPS status mode, number of satelites in view
			float nrSats = par->termToDecimal(9);    // GPS Number of satelites in view 
			float gpsStatus = par->termToDecimal(4); // GPS Status 0:No Fix, 2:2D Fix, 3:3D Fix
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = TEMP2;
			frskyBuffer[bufferLength + 2] = (byte)gpsStatus;
			frskyBuffer[bufferLength + 3] = (byte)nrSats;
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
			float altitude = (par->termToDecimal(13) - par->termToDecimal(14)) / 100.0f;
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = ALTITUDE;
			frskyBuffer[bufferLength + 2] = msByte((int)altitude);
			frskyBuffer[bufferLength + 3] = lsByte((int)altitude);
      
			unsigned int temp = (unsigned int)((altitude - (int)altitude) * 1000.0f);
    
			frskyBuffer[bufferLength + 4] = header_value;
			frskyBuffer[bufferLength + 5] = ALTIDEC;
			frskyBuffer[bufferLength + 6] = msByte(temp);
			frskyBuffer[bufferLength + 7] = lsByte(temp);
			return 8;
			break;
		}
		case GPSSPEED :
		{
			float gpsSpeed = par->termToDecimal(10) * 0.0194384f; // GPS Ground speed in knots
			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = GPSSPEED;
			frskyBuffer[bufferLength + 2] = msByte((int)gpsSpeed);
			frskyBuffer[bufferLength + 3] = lsByte((int)gpsSpeed);
      
			unsigned int temp = (unsigned int)((gpsSpeed - (int)gpsSpeed) * 1000.0f);
    
			frskyBuffer[bufferLength + 4] = header_value;
			frskyBuffer[bufferLength + 5] = GPSSPEED + decimal;
			frskyBuffer[bufferLength + 6] = msByte(temp);
			frskyBuffer[bufferLength + 7] = lsByte(temp);
      
			return 8;
			break;
		}
		case LATITUDE :
		{
			float gpsLatitude = par->termToDecimal(5)  / 10000000.0f;;

			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = LATITUDE;
			frskyBuffer[bufferLength + 2] = msByte((int)gpsLatitude);
			frskyBuffer[bufferLength + 3] = lsByte((int)gpsLatitude);
      
			unsigned int temp = (unsigned int)((gpsLatitude - (int)gpsLatitude) * 1000.0f);
    
			frskyBuffer[bufferLength + 4] = header_value;
			frskyBuffer[bufferLength + 5] = LATITUDE + decimal;
			frskyBuffer[bufferLength + 6] = msByte(temp);
			frskyBuffer[bufferLength + 7] = lsByte(temp);
      
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
			float gpsLongitude = par->termToDecimal(6);

			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = LONGITUDE;
			frskyBuffer[bufferLength + 2] = msByte((int)gpsLongitude);
			frskyBuffer[bufferLength + 3] = lsByte((int)gpsLongitude);
      
			unsigned int temp = (unsigned int)((gpsLongitude - (int)gpsLongitude) * 1000.0f);
    
			frskyBuffer[bufferLength + 4] = header_value;
			frskyBuffer[bufferLength + 5] = LONGITUDE + decimal;
			frskyBuffer[bufferLength + 6] = msByte(temp);
			frskyBuffer[bufferLength + 7] = lsByte(temp);
			
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
			float course = (par->termToDecimal(11) / 100.0f); // Course in 1/100 degree

			frskyBuffer[bufferLength] = header_value;
			frskyBuffer[bufferLength + 1] = COURSE;
			frskyBuffer[bufferLength + 2] = msByte((int)course);
			frskyBuffer[bufferLength + 3] = lsByte((int)course);
      
			unsigned int temp = (unsigned int)((course - (int)course) * 1000.0f);
    
			frskyBuffer[bufferLength + 4] = header_value;
			frskyBuffer[bufferLength + 5] = COURSE + decimal;
			frskyBuffer[bufferLength + 6] = msByte(temp);
			frskyBuffer[bufferLength + 7] = lsByte(temp);
      
			return 8;
			break;
		}
	case DATE :
	{
		float date = 0.0f;
		frskyBuffer[bufferLength] = header_value;
		frskyBuffer[bufferLength + 1] = DATE;
		frskyBuffer[bufferLength + 2] = msByte((int)date);
		frskyBuffer[bufferLength + 3] = lsByte((int)date);
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
		frskyBuffer[bufferLength + 2] = msByte((int)time);
		frskyBuffer[bufferLength + 3] = lsByte((int)time);
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
		//float accX = par->termToDecimal(18) / 100.0f;
		float accX = 0.0f;
		frskyBuffer[bufferLength] = header_value;
		frskyBuffer[bufferLength + 1] = ACCX;
		frskyBuffer[bufferLength + 2] = msByte((int)(accX*100.0f));
		frskyBuffer[bufferLength + 3] = lsByte((int)(accX*100.0f));
		return 4;
		break;
	}
	case ACCY :
	{
		//float accY = par->termToDecimal(19) / 100.0f;
		float accY = 0.0f;
		frskyBuffer[bufferLength] = header_value;
		frskyBuffer[bufferLength + 1] = ACCY;
		frskyBuffer[bufferLength + 2] = msByte((int)(accY*100.0f));
		frskyBuffer[bufferLength + 3] = lsByte((int)(accY*100.0f));
		return 4;
		break;
	}
	case ACCZ :
	{
		//float accZ = par->termToDecimal(20) / 100.0f;
		float accZ = 0.0f;
		frskyBuffer[bufferLength] = header_value;
		frskyBuffer[bufferLength + 1] = ACCZ;
		frskyBuffer[bufferLength + 2] = msByte((int)(accZ*100.0f));
		frskyBuffer[bufferLength + 3] = lsByte((int)(accZ*100.0f));
		return 4;
		break;
	}
	case CURRENT :
	{
		float current = par->termToDecimal(2);
		frskyBuffer[bufferLength] = header_value;
		frskyBuffer[bufferLength + 1] = CURRENT;
		frskyBuffer[bufferLength + 2] = msByte((int)(current*100.0f));
		frskyBuffer[bufferLength + 3] = lsByte((int)(current*100.0f));
		return 4;
		break;
	}
	case VOLTAGE :
	{
		float batteryVoltage = par->termToDecimal(1);
		frskyBuffer[bufferLength] = header_value;
		frskyBuffer[bufferLength + 1] = VOLTAGE;
		frskyBuffer[bufferLength + 2] = msByte((int)batteryVoltage);
		frskyBuffer[bufferLength + 3] = lsByte((int)batteryVoltage);
      
		unsigned int temp = (unsigned int)((batteryVoltage - (int)batteryVoltage) * 1000.0f);
    
		frskyBuffer[bufferLength + 4] = header_value;
		frskyBuffer[bufferLength + 5] = VOLTAGEDEC;
		frskyBuffer[bufferLength + 6] = msByte(temp);
		frskyBuffer[bufferLength + 7] = lsByte(temp);

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
  
	debugPort->print("GPS Alt: ");
	debugPort->print(par->termToDecimal(7), 2);
	debugPort->print(" Fuel: ");
	debugPort->print(par->termToDecimal(3), 2);
	debugPort->print(" Alt: ");
	debugPort->print(par->termToDecimal(14), 2);
	debugPort->print(" GPS Speed: ");
	debugPort->print(par->termToDecimal(10), 2);
	debugPort->print(" Latitude: ");
	debugPort->print(par->termToDecimal(5), 2);
	debugPort->print(" Longitude: ");
	debugPort->print(par->termToDecimal(6), 2);
	debugPort->print(" Current: ");
	debugPort->print(par->termToDecimal(2), 2);
	debugPort->print(" Voltage: ");
	debugPort->print(par->termToDecimal(1), 2);
	debugPort->println("");

}
