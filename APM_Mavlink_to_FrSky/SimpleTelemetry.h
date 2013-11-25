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

// The following data is sent from APM
//   $ - Header
// 0  Main battery voltage in V              
// 1  Battery current in mA                  
// 2  Battery remaining in %                 
// 3  GPS Status 0:No Fix, 2:2D Fix, 3:3D Fix
// 4  GPS Latitude in decimal degrees        
// 5  GPS Longitude in decimal degrees       
// 6  GPS Altitude in cm                     
// 7  GPS hdop                               
// 8  GPS Number of satelites in view        
// 9  GPS Ground speed in cm/s               
// 10 GPS Course in 1/100 degree                 
// 11 Altitude in cm         
// 12 Home altitude in cm    
// 13 APM mode                               
// 14 Compass                                
// 15 Throttle out           
// 16 Accel-X
// 17 Accel-Y
// 18 Accel-Z
// * - end

#ifndef simpletelemetry_h
#define simpletelemetry_h

#define MAXTERMS 21
#define MAXSENTENCE 110
#define MAXWORD 10

#include "ifrskydataprovider.h"
#include <SoftwareSerial.h>
#include <Arduino.h>
#include "defines.h"

class SimpleTelemetry :	public IFrSkyDataProvider
{
public:
	SimpleTelemetry(void);
	~SimpleTelemetry(void);
	bool		parseMessage(char c);
	const float	getGpsHdop();
	int			terms();
	char*		term(int i);
	float		termToDecimal(int t);
	const int	getGpsStatus();

	// IFrSkyDataProvider functions
	const float	getGpsAltitude();
	const int	getTemp1();
	const int	getEngineSpeed();
	const int	getFuelLevel();
	const int	getTemp2();
	const float	getAltitude();
	const float	getGpsGroundSpeed();
	const float	getLongitud();
	const float	getLatitude();
	const float	getCourse();
	const int   getYear();
	const int   getDate();
	const int	getTime();
	const float	getAccX();
	const float	getAccY();
	const float	getAccZ(); 
	const float	getBatteryCurrent();
	const float	getMainBatteryVoltage();	

private:
	int _dehex(char a);
	float gpsDdToDmsFormat(float ddm);
	SoftwareSerial* debugPort; 
	char*	f_term[MAXTERMS];
	int	f_terms;
	int	_state;
	//char	_sentence[MAXSENTENCE];
	int	n;
	int	_terms;
	char*	_term[MAXTERMS];
	int	_nt;
	int checksum;
	int _checksum;
};

#endif
