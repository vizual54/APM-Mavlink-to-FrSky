#include "FrSkySPort.h"
#include "Arduino.h"
#include "HardwareSerial.h"
#include "Aserial.h"


struct t_sportData MyData[2] ;


FrSkySPort::FrSkySPort(uint8_t pinTx)
{
  
}

// **************** Setup the FRSky OutputLib *********************
void FrSkySPort::setup()
{
	MyData[0].next = &MyData[1] ;
	initSportUart( &MyData[0] ) ;
}

#define ALT_ID       0x0100
#define VARIO_ID     0x0110

void FrSkySPort::sendValue(uint16_t id, uint32_t value)
{
	if ( id == ALT_ID )
	{
		setNewData( &MyData[0], id, value ) ;
	}
	else
	{
		setNewData( &MyData[1], id, value ) ;
	}
}

void FrSkySPort::sendData(IFrSkyDataProvider* dataProvider)
{
	static uint8_t counter ;
  switch ( counter)
	{
    case 0 :  
      sendValue(VARIO_ID, dataProvider->getTemp1());
      break;
     
    case 1 :
      sendValue(ALT_ID, dataProvider->getAltitude());
      break;
  }
  counter = (counter + 1) & 1 ;
}