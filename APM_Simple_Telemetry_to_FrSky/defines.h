#ifndef defines_h
#define defines_h

#ifndef byte
typedef unsigned char byte;
#endif



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


#endif

