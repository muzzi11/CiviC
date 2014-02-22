#pragma once

#include <vector>

#include "token.h"


class Parser
{
private:
	size_t t;
	const std::vector<Token>& tokens;
	
	void ParseDeclaration();
	void ParseFunDec();
	void ParseGlobalDec();

	void ParseDefinition();
	void ParseFunDef();
	void ParseGlobalDef();

	void ParseArray();
	void ParseParameter();

public:
	Parser(const std::vector<Token>& tokens);

	void ParseProgram();
};