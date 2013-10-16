#ifndef parser_h
#define parser_h

#define MAXTERMS 30
#define MAXSENTENCE 100
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
	SoftwareSerial* debugPort; 
	char	f_sentence[MAXSENTENCE];
	char*	f_term[MAXTERMS];
	int	f_terms;
	int	_state;
	char	_sentence[MAXSENTENCE];
	int	n;
	int	_terms;
	char*	_term[MAXTERMS];
	int	_nt;
};

#endif
