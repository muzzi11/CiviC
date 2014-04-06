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
#include "replace_boolops.h"
#include "analysis.h"
#include "assembly.h"
#include "nested_func_renaming.h"

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
		AssemblyGenerator assemblyGenerator;

		if(file.is_open())
		{
			while(tokenizer.GetNextToken(token))
			{
				tokens.push_back(token);
				//std::cout << token.readString << "\n";
			}

			std::cout << "\nToken count: " << tokens.size() << "\n";

			file.close();
		}

		try
		{
			auto root = std::make_shared<Nodes::Root>();
			parser.ParseProgram(root);

			SeperateDecAndInit(root);

			auto errors = Analyzer().Analyse(root);
			std::cout << errors;
			if (errors.size() > 0) return 0;

			ReplaceBooleanOperators(root);
			RenameNestedFunctions(root);

			std::cout << TreeToJSON(root) << "\n";
			std::cout << "-------------------------------------\n";
			std::cout << "Assembly\n";
			std::cout << "-------------------------------------\n";
			std::cout << assemblyGenerator.Generate(root) << "\n";
		}
		catch(ParseException e)
		{
			std::cout << e.what();
		}
	}
	return 0;
}
