#include <Arduino.h>
#include "parser.h"

parser::parser(void)
{
	f_terms = 0;
	_state = 0;
	n = 0;
	_terms = 0;
	_nt = 0;

        // allocate memory for individual terms of sentence in parser
	for (int t=0; t<30; t++) {
		_term[t] = (char*) malloc (15 * sizeof(char));
		f_term[t] = (char*) malloc (15 * sizeof(char));
		(f_term[t])[0] = 0;
	}
}


parser::~parser(void)
{
}

bool parser::parse(char c)
{
// LF and CR always reset parser
  if ((c == 0x0A) || (c == 0x0D))
  {
    _state = 0;
  }
  // $ Always starts a new sentence
  if (c == '$')
  {
    _sentence[0] = c;
    _state = 1;
    _terms = 0;
    n = 1;
    _nt = 0;
    return 0;
  }
  
  switch (_state)
  {
    case 0 :
    {
      // Waiting for $. Do nothing
      break;
    }
    case 1 :
    {
      _sentence[n++] = c;
      switch (c)
      {
        // Delimits the terms in sentence
        case ',' :
        {
          (_term[_terms++])[_nt] = 0;
          _nt = 0;
          break;
        }
        // End of sentence
        case '*' :
        {
          (_term[_terms++])[_nt] = 0;
          _nt = 0;
          _state++;
          break;
        }
        default :
        {
          (_term[_terms])[_nt++] = c;
          break;
        }
      }
	  break;
    }
    // Sentence complete
    case 2 :
    {
      // Copy sentence to f_sentece
      while ((--n) >= 0)
      {
        f_sentence[n] = _sentence[n];
      }
      for (f_terms=0; f_terms<_terms; f_terms++) {
      _nt = 0;
        while ((_term[f_terms])[_nt]) {
          (f_term[f_terms])[_nt] = (_term[f_terms])[_nt];
          _nt++;
        }
        (f_term[f_terms])[_nt] = 0;
        
        // Get all values from sentence
        
      }
      return 1;
    }
    default :
    {
      _state = 0;
      break;
    }
  }

  return 0;
}

float parser::termToDecimal(int t)
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
