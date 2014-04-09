#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

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
#include "replace_while.h"
#include "global_getset.h"

int main(int argc, char* argv[])
{		
	if(argc < 2)
	{
		std::cout << "No input files suplied.\n";
		return 0;
	}

	bool verbose = false;
	for(int i = 1; i < argc; ++i)
	{
		if(strcmp(argv[i], "-v") == 0) verbose = true;
	}

	for(int i = 1; i < argc; ++i)
	{
		if(strcmp(argv[i], "-v") == 0) continue;

		std::ifstream file(argv[i]);
		Tokenizer tokenizer(file);
		Token token;
		std::vector<Token> tokens;
		Parser parser(tokens);
		AssemblyGenerator assemblyGenerator;

		if(file.is_open())
		{
			try{ while(tokenizer.GetNextToken(token)) tokens.push_back(token); }
			catch(int line)
			{
				std::cout << "Integer value out of range at line " << line << "\n";
				return -1;
			}
			file.close();
		}

		try
		{
			auto root = std::make_shared<Nodes::Root>();
			parser.ParseProgram(root);

			SeperateDecAndInit(root);
			if(verbose) std::cout << "AST before:\n" << TreeToJSON(root) << "\n";

			auto errors = Analyzer().Analyse(root);
			std::cout << errors;
			if(errors.size() > 0) return 0;

			ReplaceBooleanOperators(root);
			ReplaceWhileLoops(root);
			RenameNestedFunctions(root);
			CreateGettersSetters(root);

			std::string assembly = assemblyGenerator.Generate(root);
			if(verbose)
			{
				std::cout << "AST after:\n" << TreeToJSON(root) << "\n";
				std::cout << "-------------------------------------\n";
				std::cout << "Assembly\n";
				std::cout << "-------------------------------------\n";
				std::cout << assembly << "\n";
			}

			std::string outputFileName = argv[i];
			size_t pos = outputFileName.find_last_of('.');
			outputFileName = outputFileName.substr(0, pos) + ".o";
			std::ofstream output(outputFileName, std::ios::out | std::ios::trunc);
			if(output.is_open())
			{
				output << assembly;
				output.close();
			}
			else std::cout << "Could not write to " << outputFileName << '\n';
		}
		catch(ParseException e)
		{
			std::cout << e.what();
		}
	}
	return 0;
}
