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
#pragma once

#include "ifrskydataprovider.h"
#include "SoftwareSerial.h"
#include <FastSerial.h>
#include <mavlink.h>
#include <GCS_MAVLink.h>
//#include <../Libraries/GCS_MAVLink/GCS_MAVLink.h>
//#include "../GCS_MAVLink/include/mavlink/v1.0/mavlink_types.h"
//#include "../GCS_MAVLink/include/mavlink/v1.0/ardupilotmega/mavlink.h"

class Mavlink :	public IFrSkyDataProvider
{
public:
	Mavlink(BetterStream* port);
	~Mavlink(void);
	float getMainBatteryVoltage(); 
	float getBatteryCurrent();
	int getBatteryRemaining();
	int getGpsStatus();
	float getLatitude();
	float getLongitud();
	float getGpsAltitude();
	float getGpsHdop();
	int getNumberOfSatelitesInView();
	float getGpsGroundSpeed();
	float getGpsCourse();
	float getAltitude();
	int getApmMode();
	float getCourse();
	int getThrottle();
	float getAccX();
	float getAccY();
	float getAccZ();
	int	getTime();
	int	getDate();
	bool parseMessage(char c);
	void makeRateRequest();

	bool         enable_mav_request;
	bool		 mavlink_active;
	bool		 waitingMAVBeats;

private:
	SoftwareSerial *debugPort;
	float gpsDdToDmsFormat(float ddm);
	bool		 mavbeat;
	float        lastMAVBeat;
	
	unsigned int apm_mav_type;
	unsigned int apm_mav_system; 
	unsigned int apm_mav_component;
	
	unsigned int crlf_count;
	int			 packet_drops;
	int			 parse_error;

	// Telemetry values
	float	batteryVoltage;
	float	current;
	int		batteryRemaining;
	int		gpsStatus;
	float	latitude;
	float	longitude;
	float	gpsAltitude;
	float	gpsHdop;
	int		numberOfSatelites;
	float	gpsGroundSpeed;
	float	gpsCourse;
	float	altitude;
	int		apmMode;
	int		apmBaseMode;
	float	course;
	float	throttle;
	float	accX;
	float	accY;
	float	accZ;
};
