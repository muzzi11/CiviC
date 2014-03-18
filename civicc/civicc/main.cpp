#include <iostream>
#include <fstream>
#include <vector>

#include "tokenizer.h"
#include "parser.h"

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
		Parser parser(tokens);

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

		//try
		{
			parser.ParseProgram();
		}
		//catch(ParseException e)
		{
			//std::cout << e.what();
		}
	}

	return 0;
}
