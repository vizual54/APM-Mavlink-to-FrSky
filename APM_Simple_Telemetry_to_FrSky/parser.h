#ifndef parser_h
#define parser_h

class parser
{
public:
	parser(void);
	~parser(void);
	bool parse(char c);
	float termToDecimal(int t);
private:
	char	f_sentence[100];
	char*	f_term[30];
	int		f_terms;
	int		_state;
	char	_sentence[100];
	int		n;
	int		_terms;
	char*	_term[30];
	int		_nt;
};

#endif
