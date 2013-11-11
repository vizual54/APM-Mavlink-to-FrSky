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
class IFrSkyDataProvider
{
public:
	//IFrSkyDataProvider(void);
	//~IFrSkyDataProvider(void);
	virtual float	getMainBatteryVoltage() = 0;
	virtual float	getBatteryCurrent() = 0;
	virtual int		getBatteryRemaining() = 0;
	virtual int		getGpsStatus() = 0;
	virtual float	getLatitude() = 0;
	virtual float	getLongitud() = 0;
	virtual float	getGpsAltitude() = 0;
	virtual float	getGpsHdop() = 0;
	virtual int		getNumberOfSatelitesInView() = 0;
	virtual float	getGpsGroundSpeed() = 0;
	virtual float	getGpsCourse() = 0;
	virtual float	getAltitude() = 0;
	virtual int		getApmMode() = 0;
	virtual float	getCourse() = 0;
	virtual int		getThrottle() = 0;
	virtual float	getAccX() = 0;
	virtual float	getAccY() = 0;
	virtual float	getAccZ() = 0;
	virtual int		getTime() = 0;
	virtual int		getDate() = 0;
};

