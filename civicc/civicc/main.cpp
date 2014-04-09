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
#include "replace_loops.h"
#include "global_getset.h"

int main(int argc, char* argv[])
{
	std::string inputFilename, outputFilename;
	bool verbose = false;
	
	for(int i = 1; i < argc; ++i)
	{
		if(strcmp(argv[i], "-v") == 0) verbose = true;
		else if(strcmp(argv[i], "--help") == 0)
		{
			std::cout << "civicc [-v] [-o <file>] [<file>]\n";
		}
		else if(strcmp(argv[i], "-o") == 0)
		{
			if(i + 1 >= argc)
			{
				std::cout << "No output filename supplied.\n";
				return -1;
			}
			outputFilename = argv[++i];
		}
		else
		{
			if(inputFilename.empty()) inputFilename = argv[i];
		}
	}
	
	if(inputFilename.empty())
	{
		std::cout << "No input file supplied.\n";
		return -1;
	}

	std::ifstream file(inputFilename);
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
		if(errors.size() > 0) return -1;

		ReplaceBooleanOperators(root);
		ReplaceLoops(root);
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

		if(outputFilename.empty())
		{
			if(!verbose)std::cout << assembly;
		}
		else
		{
			std::ofstream output(outputFilename, std::ios::out | std::ios::trunc);
			if(output.is_open())
			{
				output << assembly;
				output.close();
			}
			else std::cout << "Could not write to " << outputFilename << '\n';
		}
	}
	catch(ParseException e)
	{
		std::cout << e.what();
		return -1;
	}

	return 0;
}
