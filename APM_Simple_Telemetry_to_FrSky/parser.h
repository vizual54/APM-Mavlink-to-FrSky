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
