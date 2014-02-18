#include "parser.h"


Parser::Parser(std::vector<Token>& tokens) : 
	t(0),
	tokens(tokens)
{
}

void Parser::ParseProgram()
{
	while(t < tokens.size())
	{
		ParseDeclaration();
	}
}

void Parser::ParseDeclaration()
{
	if(tokens[t] == ReservedWord::Extern)
	{
		if(t + 3 < tokens.size())
		{
			const Token& type = tokens[t + 1];

			if(type == ReservedWord::Void)
			{
				ParseFunDec();
			}
			else if(type == ReservedWord::Bool || type == ReservedWord::Int || type == ReservedWord::Float)
			{
				if(tokens[t + 3] == ReservedSymbol::ParenthesesL) ParseFunDec();
				else ParseGlobalDec();
			}
		}
		else
		{
			// Unexpected end of file
		}
	}
	else
	{
		ParseDefinition();
	}
}

void Parser::ParseFunDec()
{
	const Token& type = tokens[t + 1];
	const Token& identifier = tokens[t + 2];

	if(identifier.type != TokenType::Identifier)
	{
		// Expected identifier
	}

	if(tokens[t + 3] != ReservedSymbol::ParenthesesL)
	{
		// Expected left parentheses
	}

	t += 4;

	// Moar
}

void Parser::ParseGlobalDec()
{

}

void Parser::ParseDefinition()
{
	size_t i = (tokens[t] == ReservedWord::Export) ? t + 1 : t;

	if(i + 2 >= tokens.size())
	{
		// Unexpected end of file
		return;
	}

	const Token& type = tokens[i];

	if(type == ReservedWord::Void)
	{
		ParseFunDef();
	}
	else if(type == ReservedWord::Bool || type == ReservedWord::Int || type == ReservedWord::Float)
	{
		if(tokens[i + 2] == ReservedSymbol::ParenthesesL) ParseFunDef();
		else ParseGlobalDef();
	}
	else
	{
		// Unexpected token
	}
}

void Parser::ParseFunDef()
{

}

void Parser::ParseGlobalDef()
{

}