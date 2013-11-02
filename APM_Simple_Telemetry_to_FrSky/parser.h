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

#ifndef parser_h
#define parser_h

#define MAXTERMS 21
#define MAXSENTENCE 110
#define MAXWORD 10

#include <SoftwareSerial.h>
#include <Arduino.h>
#include "defines.h"

class parser
{
public:
	parser();
	parser(SoftwareSerial* port);
	~parser(void);
	bool parse(char c);
	int terms();
	char* term(int i);
	float termToDecimal(int t);
private:
	int _dehex(char a);
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
