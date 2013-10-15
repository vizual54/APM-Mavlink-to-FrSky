#include <SoftwareSerial.h>
#include <FlexiTimer2.h>
#include "parser.h"
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
0x21   Altitude decimal
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

//#define lowByte(w) ((uint8_t) ((w) & 0xff))
//#define highByte(w) ((uint8_t) ((w) >> 8))

#define header_value   0x5e
#define tail_value     0x5e
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

#define HEARTBEATLED 13
#define HEARTBEATFREQ 500

#define DEBUG

unsigned char frskyBuffer[48];
byte bufferLength = 0;
int counter = 0;
float     gpsAltitude = 101.5f;
int16_t   temp1 = 10;
uint16_t  engineSpeed = 1024;
float  fuelLevel = 33;
int16_t   temp2 = 22;
float     altitude = 111.1f;
float     gpsSpeed = 3.3f;
float     gpsLongitude = 0.0f;
byte      eastWest = 0;
float     gpsLatitude = 0;
byte      northSouth = 0;
float     course = 359.9f;
uint16_t  dateMonth = 0;
uint16_t  year = 0;
uint16_t  hourMinute = 0;
uint16_t  second = 0;
float     accX = 2.2f;
float     accY = 3.3f;
float     accZ = -9.82f;
float     current = 0.0f;
float     batteryVoltage = 0;
parser *p;

                         //tx, rx
SoftwareSerial frSkySerial(6,5,true);
SoftwareSerial debugSerial(12,11);

long hbMillis;
byte hbState;

void setup() {
  
  p = new parser();
  digitalWrite(HEARTBEATLED, HIGH);
  hbState = HIGH;
  
  Serial.begin(57600);
  debugSerial.begin(38400);                
  frSkySerial.begin(9600);
  
  FlexiTimer2::set(200, 1.0/1000, sendFrSky); // call every 200 1ms "ticks"
  FlexiTimer2::start();
  
  // Blink fast a couple of times to show boot
  for (int i = 0; i < 20; i++)
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
    delay(100);
  }
  
  //debugSerial.println("Setup finished.");
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

void readApmData()
{
  
  while (Serial.available() > 0) {
    // Parse the motherfucker
    
    bool done = p->parse(Serial.read());
    //debugSerial.write(incomingByte);
    if (done)
    {
      // Save the values.
      // The following data is sent from APM
      // 1  $ - Header
      // 2  Main battery voltage in V                int16_t
      // 3  Battery current in mA                    int16_t
      // 4  Battery remaining in %                   int8_t
      // 5  GPS Status 0:No Fix, 2:2D Fix, 3:3D Fix  uint8_t 
      // 6  GPS Latitude in decimal degrees          int32_t
      // 7  GPS Longitude in decimal degrees         int32_t
      // 8  GPS Altitude in m                        int32_t
      // 9  GPS hdop                                 int16_t
      // 10 GPS Number of satelites in view          uint8_t
      // 11 GPS Ground speed in cm/s                 uint32_t
      // 12 Distance to arm position in m            int32_t
      // 13 latitude in dec deg                      int32_t
      // 14 longitude in dec deg                     int32_t
      // 15 altitude in m                            int32_t
      // 16 home altitude in m                       int32_t
      // 17 APM mode                                 uint8_t
      // * - end
      gpsAltitude = p->termToDecimal(8);
      fuelLevel = p->termToDecimal(4);
      altitude = p->termToDecimal(4);
      gpsSpeed = p->termToDecimal(11);
      gpsLongitude = p->termToDecimal(14);
      gpsLatitude = p->termToDecimal(13);
      current = p->termToDecimal(3);
      batteryVoltage = p->termToDecimal(2);
#ifdef DEBUG
      printValues();
#endif
    }
  }
}

void printValues()
{
  debugSerial.print("GPS Alt: ");
  debugSerial.print(gpsAltitude, 2);
  debugSerial.print("Fuel: ");
  debugSerial.print(fuelLevel, 2);
  debugSerial.print("Alt: ");
  debugSerial.print(altitude, 2);
  debugSerial.print("GPS Speed: ");
  debugSerial.print(gpsSpeed, 2);
  debugSerial.print("Latitude: ");
  debugSerial.print(gpsLatitude, 2);
  debugSerial.print("Longitude: ");
  debugSerial.print(gpsLongitude, 2);
  debugSerial.print("Current: ");
  debugSerial.print(current, 2);
  debugSerial.print("Voltage: ");
  debugSerial.print(batteryVoltage, 2);
  debugSerial.println("");
}

void sendFrSky()
{
  counter++;
  // Send 200 ms frame
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
  
  if (counter == 5) // Send 1000 ms frame
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
  if (counter == 25) // Send 5000 ms frame
  {
    // Date, Time
    /*
    bufferLength += addBufferData(DATE);
    bufferLength += addBufferData(TIME);
    frskyBuffer[++bufferLength] = tail_value;
    bufferLength = writeBuffer(bufferLength);
    */
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
      frskyBuffer[bufferLength + 1] = TEMP2;
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
    {
      frskyBuffer[bufferLength] = header_value;
      frskyBuffer[bufferLength + 1] = COURSE;
      frskyBuffer[bufferLength + 2] = msByte((int16_t)course);
      frskyBuffer[bufferLength + 3] = lsByte((int16_t)course);
      
      uint16_t temp = (uint16_t)((course - (int16_t)course) * 1000.0f);
    
      frskyBuffer[bufferLength + 4] = header_value;
      frskyBuffer[bufferLength + 5] = COURSE + decimal;
      frskyBuffer[bufferLength + 6] = msByte(temp);
      frskyBuffer[bufferLength + 7] = lsByte(temp);
      
      return 8;
      break;
    }
    case DATE :
    {
      return 0;
      break;
    }
    case YEAR :
    {
      return 0;
      break;
    }
    case TIME :
    {
      return 0;
      break;
    }
    case SECOND :
    {
      return 0;
      break;
    }
    case ACCX :
    {
      frskyBuffer[bufferLength] = header_value;
      frskyBuffer[bufferLength + 1] = ACCX;
      frskyBuffer[bufferLength + 2] = msByte((int16_t)(accX*100.0f));
      frskyBuffer[bufferLength + 3] = lsByte((int16_t)(accX*100.0f));
      return 4;
      break;
    }
    case ACCY :
    {
      frskyBuffer[bufferLength] = header_value;
      frskyBuffer[bufferLength + 1] = ACCY;
      frskyBuffer[bufferLength + 2] = msByte((int16_t)(accY*100.0f));
      frskyBuffer[bufferLength + 3] = lsByte((int16_t)(accY*100.0f));
      return 4;
      break;
    }
    case ACCZ :
    {
      frskyBuffer[bufferLength] = header_value;
      frskyBuffer[bufferLength + 1] = ACCZ;
      frskyBuffer[bufferLength + 2] = msByte((int16_t)(accZ*100.0f));
      frskyBuffer[bufferLength + 3] = lsByte((int16_t)(accZ*100.0f));
      return 4;
      break;
    }
    case CURRENT :
    {
      frskyBuffer[bufferLength] = header_value;
      frskyBuffer[bufferLength + 1] = CURRENT;
      frskyBuffer[bufferLength + 2] = msByte((int16_t)(current*100.0f));
      frskyBuffer[bufferLength + 3] = lsByte((int16_t)(current*100.0f));
      return 4;
      break;
    }
    case VOLTAGE :
    {
      frskyBuffer[bufferLength] = header_value;
      frskyBuffer[bufferLength + 1] = VOLTAGE;
      frskyBuffer[bufferLength + 2] = msByte((int16_t)batteryVoltage);
      frskyBuffer[bufferLength + 3] = lsByte((int16_t)batteryVoltage);
      
      uint16_t temp = (uint16_t)((batteryVoltage - (int16_t)batteryVoltage) * 1000.0f);
    
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

byte writeBuffer(byte length)
{
  int i = 0;
  while(i < length)
  {
    // If a data value is equal to header (0x5E), tail (0x5E) or escape (0x5D) value exchange it.
    // There is always two bytes between header and tail
    if ((i % 4))
    {
      switch (frskyBuffer[i])
      {
        case header_value :
        {
          frSkySerial.write(byte(0x5D));
          frSkySerial.write(byte(0x3E));
#ifdef DEBUG
          debugSerial.write(byte(0x5D));
          debugSerial.write(byte(0x3E));
#endif
          break;
        }
        case escape_value :
        {
          frSkySerial.write(byte(0x5D));
          frSkySerial.write(byte(0x3D));
#ifdef DEBUG
          debugSerial.write(byte(0x5D));
          debugSerial.write(byte(0x3D));
#endif
          break;
        }
        default :
        {
          frSkySerial.write((byte)frskyBuffer[i]);
#ifdef DEBUG
          debugSerial.write((byte)frskyBuffer[i]);
#endif
        }
      }
    }
    else
    {
      frSkySerial.write((byte)frskyBuffer[i]);
#ifdef DEBUG
      debugSerial.write((byte)frskyBuffer[i]);
#endif
    }
    
    i++;
  }

  return 0;
}


