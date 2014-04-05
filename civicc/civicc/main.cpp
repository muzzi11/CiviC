#include <iostream>
#include <fstream>
#include <vector>

#include "tokenizer.h"
#include "parser.h"
#include "instruction.h"
#include "node.h"
#include "traverse.h"
#include "symboltable.h"
#include "seperation.h"


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

		try
		{
			auto root = std::make_shared<Node::BaseNode>();
			parser.ParseProgram(root);

			SeperateVarDecFromInit(root);
			
			std::cout << TreeToJSON(root) << "\n";
		}
		catch(ParseException e)
		{
			std::cout << e.what();
		}
	}

	SymbolTable::Record record = SymbolTable::Record();

	SymbolTable::Sheaf sheaf = SymbolTable::Sheaf();
	sheaf.InitializeScope();
	sheaf.Insert("a", record);
	sheaf.InitializeScope();
	sheaf.Insert("b", record);
	sheaf.FinalizeScope();
	sheaf.Insert("b", record);
	sheaf.FinalizeScope();

	return 0;
}
