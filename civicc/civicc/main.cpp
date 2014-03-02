#include <iostream>
#include <fstream>
#include <vector>

#include "tokenizer.h"

bool scanner(const char* str);

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		std::cout << "No input files suplied.\n";
		return 0;
	}

	for(int i = 1; i < argc; ++i)
	{
		std::ifstream file(argv[i]);
		Tokenizer tokenizer(file);
		Token token;
		std::vector<Token> tokens;

		if(file.is_open())
		{
			while(tokenizer.GetNextToken(token))
			{
				tokens.push_back(token);
				std::cout << token.readString << "\n";
			}

			std::cout << "\nToken count: " << tokens.size() << "\n";

			file.close();
		}
	}

	std::string word;
	for(;;)
	{
		std::cin >> word;
		std::cout << scanner(word.c_str()) << "\n";
	}
	return 0;
}
/*
Assignment 12


bool Start()
{
	return Expr() && NextToken() == eof();
}

bool Expr()
{
	return Term() && ExprP();
}

bool ExprP()
{
	token = nextToken();
	
	if(token == plus) return Term() && ExprP();
	
	ungetToken(token);
	return true;
}

bool Term()
{
	token = nextToken();
	
	if(token == minus) return Term();
	else return Suffix();
}

bool Suffix()
{
	return Identifier() && SuffixP();
}

bool SuffixP()
{
	token = nextToken();

	switch(token)
	{
	case minus:
		return SuffixPM();
	case plus:
		return SuffixPP();
	default:
		ungetToken(token);
		return true;
	}
}

bool SuffixPM()
{
	token = nextToken();
	if(token == minus) return SuffixP();
	
	return false;
}

bool SuffixPP()
{
	token = nextToken();
	if(token == plus) return SuffixP();

	return false;
}
*/

bool scanner(const char* word)
{
	int pos = 0;
	int len = strlen(word);

state_a:
	if(len == 0) return true;
	if(word[pos] == 'a' || word[pos] == 'b')
	{
		pos++;
		goto state_b;
	}
	else
	{
		return false;
	}

state_b:
	if(word[pos++] == 'c') goto state_c;
	else return false;
state_c:
	if(pos == len) return true;
	
	if(word[pos] == 'a' || word[pos] == 'b')
	{
		pos++;
		goto state_b;
	}

	return false;
}