#pragma once

#include <vector>

#include "token.h"


class Parser
{
private:
	size_t t;
	std::vector<Token>& tokens;
	
	void ParseDeclaration();
	void ParseFunDec();
	void ParseGlobalDec();

	void ParseDefinition();
	void ParseFunDef();
	void ParseGlobalDef();

public:
	Parser(std::vector<Token>& tokens);

	void ParseProgram();
};