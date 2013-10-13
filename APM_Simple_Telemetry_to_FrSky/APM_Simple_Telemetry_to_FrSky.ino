/*
0x01  GPS Altitude          M     int16
0x02  Temperature1          C     int16
0x03  Engine speed          RPM   uint16
0x04  Fuel Level            %     uint16
0x05  Temperature2          C     int16
0x06  Cell voltage          V     ?
0x07  
0x08
0x09  GPS Altitude decimal  .M    int16
0x0A
0x0B
0x0C
0x0D
0x0E
0x0F
0x10   Altitude		    M     int16
0x11   GPS Speed            Knots uint16
0x12   GPS Longitude        ddmm
0x13   GPS Latitude         ddmm
0x14   Course               Degree uint16
0x15   Date/Month
0x16   Year
0x17   Hour/Minute
0x18   Second
0x19   GPS Speed decimal    .knots
0x1A   GPS Longitude dec    .mmmm
0x1B   GPS Latitude  dec    .mmmm
0x1C   Course decimal       .degree
0x1D
0x1E
0x1F
0x20
0x21
0x22   E/W
0x23   N/S            
0x24   Acc-X          G   int16
0x25   Acc-Y          G   int16
0x26   Acc-Z          G   int16
0x27
0x28   Current        A   uint16
.
0x3A   Battery voltage    uint16
.
0x3B   Battery voltage decimal

*/

//#include <FlexiTimer2.h>

//#define lowByte(w) ((uint8_t) ((w) & 0xff))
//#define highByte(w) ((uint8_t) ((w) >> 8))

#define header_value   0x5e
#define escape_value   0x5d
#define decimal        0x8
      
#define GPSALT         0x1
#define TEMP1          0x2
#define RPM            0x3
#define FUEL           0x4
#define TEMP2          0x5
#define INDVOLT        0x6
#define ALTITUDE       0x10
#define GPSSPEED       0x11
#define LONGITUDE      0x12
#define LATITUDE       0x13
#define COURSE         0x14
#define DATE           0x15
#define YEAR           0x16
#define TIME           0x17
#define SECOND         0x18
#define ALTIDEC        0x21
#define EASTWEST       0x22
#define NORTHSOUTH     0x23
#define ACCX           0x24
#define ACCY           0x25
#define ACCZ           0x26
#define CURRENT        0x28
#define VOLTAGE        0x3A
#define VOLTAGEDEC     0x3B

unsigned char frskyBuffer[48];
byte bufferLength = 0;
int counter = 0;
float gpsAltitude = 0.0f;
int16_t temp1 = 0;
uint16_t engineSpeed = 0;
uint16_t  fuelLevel = 0;
int16_t temp2 = 0;
float altitude = 0.0f;
float gpsSpeed = 0.0f;
float gpsLongitude = 0.0f;
byte eastWest = 0;
float gpsLatitude = 0;
byte northSouth = 0;
uint16_t course = 0;
uint16_t dateMonth = 0;
uint16_t year = 0;
uint16_t hourMinute = 0;
uint16_t second = 0;
int16_t accX = 0;
int16_t accY = 0;
int16_t accZ = 0;
uint16_t current = 0;
int16_t batteryVoltage = 0;


void sendFrSky()
{
  counter++;
  // Send 200 ms frame
  
  bufferLength += addBufferData(ACCX);
  bufferLength += addBufferData(ACCY);
  bufferLength += addBufferData(ACCZ);
  bufferLength += addBufferData(ALTITUDE);
  bufferLength += addBufferData(TEMP1);
  bufferLength += addBufferData(TEMP2);
  bufferLength += addBufferData(INDVOLT);
  bufferLength += addBufferData(CURRENT);
  bufferLength += addBufferData(VOLTAGE);
  bufferLength += addBufferData(VOLTAGEDEC);
  bufferLength += addBufferData(RPM);
  bufferLength = writeBuffer(bufferLength);
  
  if (counter == 5) // Send 1000 ms frame
  {
    
  }
  if (counter == 25) // Send 5000 ms frame
  {
    counter = 0;
  }
}

byte lsByte(int16_t value)
{
  return ((uint8_t) ((value) & 0xff));
}

byte msByte(int16_t value)
{
  return ((uint8_t) ((value) >> 8));
}

byte addBufferData(const char id)
{
  int buffLength = 0;
  switch(id) {
    case GPSALT :
    {
      frskyBuffer[bufferLength] = header_value;
      frskyBuffer[bufferLength + 1] = GPSALT;
      frskyBuffer[bufferLength + 2] = msByte(gpsAltitude);
      frskyBuffer[bufferLength + 3] = lsByte(gpsAltitude);
      
      uint16_t temp = (uint16_t)((gpsAltitude - (int16_t)gpsAltitude) * 1000.0f);

      frskyBuffer[bufferLength + 4] = header_value;
      frskyBuffer[bufferLength + 5] = GPSALT + decimal;
      frskyBuffer[bufferLength + 6] = msByte(temp);
      frskyBuffer[bufferLength + 7] = lsByte(temp);
      
      return 8;
      break;
    }
    case TEMP1 :
    {
      frskyBuffer[bufferLength] = header_value;
      frskyBuffer[bufferLength + 1] = TEMP1;
      frskyBuffer[bufferLength + 2] = msByte(temp1);
      frskyBuffer[bufferLength + 3] = lsByte(temp1);
      
      return 4;
      break;
    }
    case RPM :
    {
      frskyBuffer[bufferLength] = header_value;
      frskyBuffer[bufferLength + 1] = RPM;
      frskyBuffer[bufferLength + 2] = msByte(engineSpeed);
      frskyBuffer[bufferLength + 3] = lsByte(engineSpeed);
      return 4;
      break;
    }
    case FUEL :
    {
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
      frskyBuffer[bufferLength] = header_value;
      frskyBuffer[bufferLength + 1] = TEMP1;
      frskyBuffer[bufferLength + 2] = msByte(temp2);
      frskyBuffer[bufferLength + 3] = lsByte(temp2);
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
      frskyBuffer[bufferLength] = header_value;
      frskyBuffer[bufferLength + 1] = ALTITUDE;
      frskyBuffer[bufferLength + 2] = msByte((int16_t)altitude);
      frskyBuffer[bufferLength + 3] = lsByte((int16_t)altitude);
      
      uint16_t temp = (uint16_t)((altitude - (int16_t)altitude) * 1000.0f);
    
      frskyBuffer[bufferLength + 4] = header_value;
      frskyBuffer[bufferLength + 5] = ALTIDEC;
      frskyBuffer[bufferLength + 6] = msByte(temp);
      frskyBuffer[bufferLength + 7] = lsByte(temp);
      return 8;
      break;
    }
    case GPSSPEED :
    {
    
      frskyBuffer[bufferLength] = header_value;
      frskyBuffer[bufferLength + 1] = GPSSPEED;
      frskyBuffer[bufferLength + 2] = msByte((int16_t)gpsSpeed);
      frskyBuffer[bufferLength + 3] = lsByte((int16_t)gpsSpeed);
      
      uint16_t temp = (uint16_t)((gpsSpeed - (int16_t)gpsSpeed) * 1000.0f);
    
      frskyBuffer[bufferLength + 4] = header_value;
      frskyBuffer[bufferLength + 5] = GPSSPEED + decimal;
      frskyBuffer[bufferLength + 6] = msByte(temp);
      frskyBuffer[bufferLength + 7] = lsByte(temp);
      
      return 8;
      break;
    }
    case LONGITUDE :
    {
      frskyBuffer[bufferLength] = header_value;
      frskyBuffer[bufferLength + 1] = LONGITUDE;
      frskyBuffer[bufferLength + 2] = msByte((int16_t)gpsLongitude);
      frskyBuffer[bufferLength + 3] = lsByte((int16_t)gpsLongitude);
      
      uint16_t temp = (uint16_t)((gpsLongitude - (int16_t)gpsLongitude) * 1000.0f);
    
      frskyBuffer[bufferLength + 4] = header_value;
      frskyBuffer[bufferLength + 5] = LONGITUDE + decimal;
      frskyBuffer[bufferLength + 6] = msByte(temp);
      frskyBuffer[bufferLength + 7] = lsByte(temp);
      
      frskyBuffer[bufferLength + 8] = header_value;
      frskyBuffer[bufferLength + 9] = EASTWEST;
      frskyBuffer[bufferLength + 10] = msByte(eastWest);
      frskyBuffer[bufferLength + 11] = lsByte(eastWest);
      
      return 12;
      break;
    }
    case LATITUDE :
    {
      frskyBuffer[bufferLength] = header_value;
      frskyBuffer[bufferLength + 1] = LATITUDE;
      frskyBuffer[bufferLength + 2] = msByte((int16_t)gpsLatitude);
      frskyBuffer[bufferLength + 3] = lsByte((int16_t)gpsLatitude);
      
      uint16_t temp = (uint16_t)((gpsLatitude - (int16_t)gpsLatitude) * 1000.0f);
    
      frskyBuffer[bufferLength + 4] = header_value;
      frskyBuffer[bufferLength + 5] = LATITUDE + decimal;
      frskyBuffer[bufferLength + 6] = msByte(temp);
      frskyBuffer[bufferLength + 7] = lsByte(temp);
      
      frskyBuffer[bufferLength + 8] = header_value;
      frskyBuffer[bufferLength + 9] = NORTHSOUTH;
      frskyBuffer[bufferLength + 10] = msByte(northSouth);
      frskyBuffer[bufferLength + 11] = lsByte(northSouth);
      
      return 12;
      break;
    }
    case COURSE :
    break;
    case DATE :
    break;
    case YEAR :
    break;
    case TIME :
    break;
    case SECOND :
    break;
    case ACCX :
    break;
    case ACCY :
    break;
    case ACCZ :
    break;
    case CURRENT :
    break;
    case VOLTAGE :
    break;
    default :
      return 0;
  }
  return 0;
}

byte writeBuffer(byte length)
{
  return 0;
}

void setup() {
  //FlexiTimer2::set(200, 1.0/1000, sendFrSky); // call every 200 1ms "ticks"
  //FlexiTimer2::set(500, flash); // MsTimer2 style is also supported
  //FlexiTimer2::start();
}

void loop() {
  // put your main code here, to run repeatedly: 
  
}



