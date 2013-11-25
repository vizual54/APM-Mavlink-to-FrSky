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

#include "SimpleTelemetry.h"

SimpleTelemetry::SimpleTelemetry(void)
{
	f_terms = 0;
	_state = 0;
	n = 0;
	_terms = 0;
	_nt = 0;

        // allocate memory for individual terms of sentence in parser
	for (int t=0; t<MAXTERMS; t++) {
		_term[t] = (char*) malloc (MAXWORD * sizeof(char));
		f_term[t] = (char*) malloc (MAXWORD * sizeof(char));
		//(f_term[t])[0] = 0;
		for (int c = 0; c < MAXWORD; c++)
		{
			(f_term[t])[c] = 0;
		}
	}
}

SimpleTelemetry::~SimpleTelemetry(void)
{
}

bool SimpleTelemetry::parseMessage(char c)
{
	// Dont let sentences run away
	if ((n >= MAXSENTENCE) || (_terms >= MAXTERMS) || (_nt >= MAXWORD))
	{
/*
#ifdef DEBUG
		debugPort->println("Runaway sentance. Resetting state. ");
		debugPort->print("Sentence is: ");
		debugPort->print(n);
		debugPort->print(" chars ");
		debugPort->print(_terms);
		debugPort->print(" terms ");
		debugPort->print(_nt);
		debugPort->println(" words.");
#endif
*/
		_state = 0;
		n = 0;
		_terms = 0;
		_nt = 0;
	}
	// LF and CR always reset parser
	if ((c == 0x0A) || (c == 0x0D))
	{
/*
#ifdef DEBUG
		debugPort->println("LF or CR received. Resetting state.");
#endif
*/
		_state = 0;
	}
	// $ Always starts a new sentence
	if (c == '$')
	{
/*
#ifdef DEBUG
	debugPort->println("New sentence.");
#endif
*/
		//_sentence[0] = c;
		_state = 1;
		_terms = 0;
		n = 1;
		_nt = 0;
		checksum = 0;
		return 0;
	}
  
	switch (_state)
	{
		case 0 :
		{
			// Waiting for $. Do nothing
/*
#ifdef DEBUG
			debugPort->println("Waiting for $.");
#endif
*/
			break;
		}
		case 1 :
		{
			//_sentence[n++] = c;
			n++;
			switch (c)
			{
			// Delimits the terms in sentence
				case ',' :
				{
/*
#ifdef DEBUG
					debugPort->print(",");
#endif
*/
					(_term[_terms++])[_nt] = 0;
					_nt = 0;
					checksum = checksum ^ c;
					break;
				}
				// End of terms
				case '*' :
				{
/*
#ifdef DEBUG
					debugPort->print("*");
#endif
*/
					(_term[_terms++])[_nt] = 0;
					_nt = 0;
					_state++;
					break;
				}
				// All characters between $ and *
				default :
				{
/*
#ifdef DEBUG
					debugPort->print(c);
#endif
*/
					(_term[_terms])[_nt++] = c;
					checksum = checksum ^ c;
					break;
				}
			}
			break;
		}
		// Checksum MSB
		case 2 :
		{
/*
#ifdef DEBUG
			debugPort->print(c);
#endif
*/
			checksum = checksum - (16 * _dehex(c));
			_state++;
			break;
		}
		// Sentence complete
		case 3 :
		{
/*
#ifdef DEBUG
			debugPort->println(c);
#endif
*/
			checksum = checksum - _dehex(c);
/*
#ifdef DEBUG
			debugPort->print("Scentence complete. Checksum is: ");
			debugPort->println(checksum, HEX);
#endif
*/
			if (checksum == 0)
			{
				for (f_terms=0; f_terms<_terms; f_terms++)
				{
					_nt = 0;
					while ((_term[f_terms])[_nt])
					{
						(f_term[f_terms])[_nt] = (_term[f_terms])[_nt];
						_nt++;
					}
					(f_term[f_terms])[_nt] = 0;        
				}

				_state = 0;
				return 1;
			}
			break;
		}
		default :
		{
			_state = 0;
			break;
		}
  }

  return 0;
}

const float SimpleTelemetry::getMainBatteryVoltage()
{
	return termToDecimal(0);
}

const float SimpleTelemetry::getBatteryCurrent()
{
	return termToDecimal(1) / 1000.0f;
}

const int SimpleTelemetry::getFuelLevel()
{
	return (int)termToDecimal(2); 
}

const int SimpleTelemetry::getGpsStatus()
{
	return (int)termToDecimal(3); // GPS Status 0:No Fix, 2:2D Fix, 3:3D Fix
}

const float SimpleTelemetry::getLatitude()
{
	return gpsDdToDmsFormat(termToDecimal(4) / 10000000.0f);
}

const float SimpleTelemetry::getLongitud()
{
	return gpsDdToDmsFormat(termToDecimal(5) / 10000000.0f);
}

const float SimpleTelemetry::getGpsAltitude()
{
	return termToDecimal(6) / 100.0f;
}

const float SimpleTelemetry::getGpsHdop()
{
	return termToDecimal(7) / 100.0f;
}

const int SimpleTelemetry::getTemp2()
{
	// GPS Status 0:No Fix, 2:2D Fix, 3:3D Fix + GPS Number of satelites in view 
	return (int)termToDecimal(3) * 10 + (int)termToDecimal(8);
}

const float SimpleTelemetry::getGpsGroundSpeed()
{
	return termToDecimal(9) * 0.0194384f; // Ground speed in knots
}

const float SimpleTelemetry::getAltitude()
{
	return (termToDecimal(11) - termToDecimal(12)) / 100.0f;
}

const int SimpleTelemetry::getTemp1()
{
	return (int)termToDecimal(13);
}

const float SimpleTelemetry::getCourse()
{
	return termToDecimal(14) / 100.0f; // Course in 1/100 degree
}

const int SimpleTelemetry::getEngineSpeed()
{
	return (int)termToDecimal(15);
}

const float SimpleTelemetry::getAccX()
{
	return termToDecimal(16) / 100.0f;
}
	
const float SimpleTelemetry::getAccY()
{
	return termToDecimal(17) / 100.0f;
}

const float SimpleTelemetry::getAccZ()
{
	return termToDecimal(18) / 100.0f;
}

const int SimpleTelemetry::getYear()
{
	return 0;
}

const int SimpleTelemetry::getTime()
{
	return 0;
}

const int SimpleTelemetry::getDate()
{
	return 0;
}

int SimpleTelemetry::terms()
{
	return f_terms;
}

char* SimpleTelemetry::term(int i)
{
	return f_term[i];
}

// We receive the GPS coordinates in ddd.dddd format
// FrSky wants the dd mm.mmm format so convert.
float SimpleTelemetry::gpsDdToDmsFormat(float ddm)
{
	int deg = (int)ddm;
	float min_dec = (ddm - deg) * 60.0f;
	float sec = (min_dec - (int)min_dec) * 60.0f;

	return (float)deg * 100.0f + (int)min_dec + sec / 100.0f;
}

float SimpleTelemetry::termToDecimal(int t)
{
	char *s = f_term[t];
	long  rl = 0;
	float rr = 0.0f;
	float rb = 0.1f;
	bool dec = false;
	int i = 0;
  
	if ((s[i] == '-') || (s[i] == '+'))
	{
		i++;
	}
  
	while (s[i] != 0)
	{
		if (s[i] == '.')
		{
			dec = true;
		}
		else
		{
			if (!dec) {
			rl = (10 * rl) + (s[i] - 48);
			}
			else
			{
				rr += rb * (float)(s[i] - 48);
				rb /= 10.0;
			}
		}

		i++;
	}

	rr += (float)rl;
	
	if (s[0] == '-')
	{
		rr = 0.0f - rr;
	}
	
	return rr;
}

int SimpleTelemetry::_dehex(char a) {
	// returns base-16 value of chars '0'-'9' and 'A'-'F';
	// does not trap invalid chars!
  if (int(a) >= 65) {
    return int(a)-87;
  }
  else {
    return int(a)-48;
  }
}