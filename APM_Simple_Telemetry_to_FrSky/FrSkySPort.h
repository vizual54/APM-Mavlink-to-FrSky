#pragma once
#ifndef FRSKYSPORT_h
#define FRSKYSPORT_h

#include "Arduino.h"
#include "IFrSkyDataProvider.h"

class FrSkySPort
{
public:
	FrSkySPort(uint8_t pinTx);
    void setup();
    bool timeToSend();
    void sendData(IFrSkyDataProvider* dataProvider);
    
private:
    uint8_t _pinTx;
    void sendValue(uint16_t id, uint32_t value) ;
};

#endif