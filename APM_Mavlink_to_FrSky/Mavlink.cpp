/*
	@author 	Nils Högberg
	@contact 	nils.hogberg@gmail.com

	Original code from https://code.google.com/p/arducam-osd/wiki/arducam_osd

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

#include "Mavlink.h"

//#include "include/mavlink/v1.0/mavlink_types.h"
//#include "include/mavlink/v1.0/ardupilotmega/mavlink.h"

//#include "../Libraries/GCS_MAVLink/GCS_MAVLink.h"
//#include "../GCS_MAVLink/include/mavlink/v1.0/mavlink_types.h"
//#include "../GCS_MAVLink/include/mavlink/v1.0/ardupilotmega/mavlink.h"



Mavlink::Mavlink(BetterStream* port)
{
	mavlink_comm_0_port = port;
	mavlink_system.sysid = 12;
	mavlink_system.compid = 1;
	mavlink_system.type = 0;
	mavlink_system.state = 0;
	
	lastMAVBeat = 0;
	mavlink_active = 0;
	waitingMAVBeats = 0;
	enable_mav_request = 1;

	batteryVoltage = 0;
	current = 0;
	batteryRemaining = 0;
	gpsStatus = 0;
	latitude = 0;
	longitude = 0;
	gpsAltitude = 0;
	gpsHdop = 0;
	numberOfSatelites = 0;
	gpsGroundSpeed = 0;
	gpsCourse = 0;
	altitude = 0;
	apmMode = 0;
	course = 0;
	throttle = 0;
	accX = 0;
	accY = 0;
	accZ = 0;
}

Mavlink::~Mavlink(void)
{
}

const float Mavlink::getMainBatteryVoltage()
{
	return batteryVoltage;
}

const float Mavlink::getBatteryCurrent()
{
	return current / 1000.0f;
}

const int Mavlink::getFuelLevel()
{
	return batteryRemaining;
}

const int Mavlink::getGpsStatus()
{
	return gpsStatus;
}

const float Mavlink::getLatitude()
{
	return gpsDdToDmsFormat(latitude / 10000000.0f);
}

const float Mavlink::getLongitud()
{
	return gpsDdToDmsFormat(longitude / 10000000.0f);
}

const float Mavlink::getGpsAltitude()
{
	return gpsAltitude / 100.0f;
}

const float Mavlink::getGpsHdop()
{
	return gpsHdop / 100.0f;
}

const int Mavlink::getTemp2()
{	
	return gpsStatus * 10 + numberOfSatelites;
}

const float Mavlink::getGpsGroundSpeed()
{
	return gpsGroundSpeed * 0.0194384f;
}

const float Mavlink::getAltitude()
{
	return altitude / 100.0f;
}

const int Mavlink::getTemp1()
{
	return apmMode;
}

const float Mavlink::getCourse()
{
	return course;
}

const int Mavlink::getEngineSpeed()
{
	return throttle;
}

const float Mavlink::getAccX()
{
	return accX;
}
	
const float Mavlink::getAccY()
{
	return accY;
}

const float Mavlink::getAccZ()
{
	return accZ;
}

const int Mavlink::getYear()
{
	return 0;
}

const int Mavlink::getTime()
{
	return 0;
}

const int Mavlink::getDate()
{
	return 0;
}

// We receive the GPS coordinates in ddd.dddd format
// FrSky wants the dd mm.mmm format so convert.
float Mavlink::gpsDdToDmsFormat(float ddm)
{
	int deg = (int)ddm;
	float min_dec = (ddm - deg) * 60.0f;
	float sec = (min_dec - (int)min_dec) * 60.0f;

	return (float)deg * 100.0f + (int)min_dec + sec / 100.0f;
}

void Mavlink::makeRateRequest()
{
	const int  maxStreams = 6;
    const unsigned short MAVStreams[maxStreams] = {MAV_DATA_STREAM_RAW_SENSORS,
        MAV_DATA_STREAM_EXTENDED_STATUS,
        MAV_DATA_STREAM_RC_CHANNELS,
        MAV_DATA_STREAM_POSITION,
        MAV_DATA_STREAM_EXTRA1, 
        MAV_DATA_STREAM_EXTRA2};
    const unsigned int MAVRates[maxStreams] = {0x02, 0x02, 0x05, 0x02, 0x05, 0x02};
    for (int i=0; i < maxStreams; i++) {
        mavlink_msg_request_data_stream_send(MAVLINK_COMM_0,
            apm_mav_system, apm_mav_component,
            MAVStreams[i], MAVRates[i], 1);
    }
}

bool Mavlink::parseMessage(char c)
{
	mavlink_message_t msg; 
    mavlink_status_t status;

	// allow CLI to be started by hitting enter 3 times, if no heartbeat packets have been received
	if (mavlink_active == 0 && millis() < 20000 && millis() > 5000)
	{
		if (c == '\n' || c == '\r')
		{
			crlf_count++;
		}
		else 
		{
			crlf_count = 0;
		}
	}

    //trying to grab msg  
	if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) 
	{
            mavlink_active = 1;
			switch(msg.msgid) {
            case MAVLINK_MSG_ID_HEARTBEAT:
            {
                mavbeat = 1;
                apm_mav_system    = msg.sysid;
                apm_mav_component = msg.compid;
                apm_mav_type      = mavlink_msg_heartbeat_get_type(&msg);            
                apmMode			  = (unsigned int)mavlink_msg_heartbeat_get_custom_mode(&msg);
				apmBaseMode		  = mavlink_msg_heartbeat_get_base_mode(&msg);

                //if(getBit(base_mode,7)) motor_armed = 1;
                //else motor_armed = 0;

                //osd_nav_mode = 0;          
                lastMAVBeat = millis();
                if(waitingMAVBeats == 1)
				{
                    enable_mav_request = 1;
                }
				return true;
				break;
            }
			case MAVLINK_MSG_ID_SYS_STATUS:
            {
                batteryVoltage = (mavlink_msg_sys_status_get_voltage_battery(&msg) / 1000.0f); //Battery voltage, in millivolts (1 = 1 millivolt)
                current = mavlink_msg_sys_status_get_current_battery(&msg); //Battery current, in 10*milliamperes (1 = 10 milliampere)         
                batteryRemaining = mavlink_msg_sys_status_get_battery_remaining(&msg); //Remaining battery energy: (0%: 0, 100%: 100)
				return true;
				break;
            }
            case MAVLINK_MSG_ID_GPS_RAW_INT:
            {
                latitude = mavlink_msg_gps_raw_int_get_lat(&msg);
                longitude = mavlink_msg_gps_raw_int_get_lon(&msg);
                gpsStatus = mavlink_msg_gps_raw_int_get_fix_type(&msg);
                numberOfSatelites = mavlink_msg_gps_raw_int_get_satellites_visible(&msg);
				gpsHdop = mavlink_msg_gps_raw_int_get_eph(&msg);
				gpsAltitude = mavlink_msg_gps_raw_int_get_alt(&msg);
				gpsCourse = mavlink_msg_gps_raw_int_get_cog(&msg);
				return true;
				break;
			}
            case MAVLINK_MSG_ID_VFR_HUD:
            {
                //osd_airspeed = mavlink_msg_vfr_hud_get_airspeed(&msg);
                gpsGroundSpeed = mavlink_msg_vfr_hud_get_groundspeed(&msg);
                course = mavlink_msg_vfr_hud_get_heading(&msg); // 0..360 deg, 0=north
                throttle = mavlink_msg_vfr_hud_get_throttle(&msg);
                //if(osd_throttle > 100 && osd_throttle < 150) osd_throttle = 100;//Temporary fix for ArduPlane 2.28
                //if(osd_throttle < 0 || osd_throttle > 150) osd_throttle = 0;//Temporary fix for ArduPlane 2.28
                altitude = mavlink_msg_vfr_hud_get_alt(&msg);
                
				break;
            }
            case MAVLINK_MSG_ID_ATTITUDE:
            {
                accX = ToDeg(mavlink_msg_attitude_get_pitch(&msg));
                accY = ToDeg(mavlink_msg_attitude_get_roll(&msg));
                accZ = ToDeg(mavlink_msg_attitude_get_yaw(&msg));
				break;
            }
            case MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT:
			{
				//nav_roll = mavlink_msg_nav_controller_output_get_nav_roll(&msg);
				//nav_pitch = mavlink_msg_nav_controller_output_get_nav_pitch(&msg);
				//nav_bearing = mavlink_msg_nav_controller_output_get_nav_bearing(&msg);
				//wp_target_bearing = mavlink_msg_nav_controller_output_get_target_bearing(&msg);
				//wp_dist = mavlink_msg_nav_controller_output_get_wp_dist(&msg);
				//alt_error = mavlink_msg_nav_controller_output_get_alt_error(&msg);
				//aspd_error = mavlink_msg_nav_controller_output_get_aspd_error(&msg);
				//xtrack_error = mavlink_msg_nav_controller_output_get_xtrack_error(&msg);
				break;
			}
			
			default:
                //Do nothing
                break;
            }
	}
	// Update global packet drops counter
    packet_drops += status.packet_rx_drop_count;
    parse_error += status.parse_error;
	return false;
}
