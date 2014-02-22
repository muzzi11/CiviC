#include "parser.h"


Parser::Parser(const std::vector<Token>& tokens) : 
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
	if(t + 4 >= tokens.size())
	{
		// Unexpected end of file
		return;
	}

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

	while(tokens[t] != ReservedSymbol::ParenthesesR)
	{
		ParseParameter();
		if(tokens[t] == ReservedSymbol::Comma) t++;

		if(t >= tokens.size())
		{
			// Unexpected end of file
			return;
		}
	}

	if(tokens[++t] == ReservedSymbol::Semicolon)
	{
		// Do stuff
	}
	else
	{
		// Expected a semicolon
	}
}

void Parser::ParseGlobalDec()
{
	if(t + 3 >= tokens.size())
	{
		// Unexpected end of file
		return;
	}

	const Token& identifier = tokens[t + 2];
	const Token& semicolon = tokens[t + 3];

	t += 4;

	if(identifier.type == TokenType::Identifier)
	{
		if(semicolon == ReservedSymbol::Semicolon)
		{
			// Do stuff
		}
		else
		{
			// Expected a semicolon
		}
	}
	else
	{
		// Expected identifier
	}
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

void Parser::ParseArray()
{
	if(t + 3 >= tokens.size())
	{
		// Unexpected end of file
		return;
	}

	const Token& type = tokens[t];
	const Token& identifier = tokens[t+1];
	
	if(type == ReservedWord::Bool || type == ReservedWord::Int || type == ReservedWord::Float)
	{

	}
	else
	{
		// Unexpected token
	}

	if(identifier.type != TokenType::Identifier)
	{
		// Expected an identifier
	}

	if(tokens[t + 2] != ReservedSymbol::BracketL)
	{
		// Expected a left bracket
	}

	t += 3;
	
	while(tokens[t] != ReservedSymbol::BracketR)
	{
		if(tokens[t].type == TokenType::Identifier)
		{
			t++;
			// Do stuff
		}
		else if(tokens[t].type == TokenType::IntType)
		{
			t++;
			// Do stuff
		}
		else
		{
			// Unexpected token
		}

		if(tokens[t] == ReservedSymbol::Comma) t++;

		if(t >= tokens.size())
		{
			// Unexpected end of file
			return;
		}
	}
}

void Parser::ParseParameter()
{
	if(t + 2 >= tokens.size())
	{
		// unexpected end of file
		return;
	}

	const Token& type = tokens[t];
	const Token& identifier = tokens[t + 1];
	bool isArray = tokens[t + 2] == ReservedSymbol::BracketL;

	if(isArray)
	{
		ParseArray();
	}
	else
	{
		if(type == ReservedWord::Bool || type == ReservedWord::Int || type == ReservedWord::Float)
		{
			if(identifier.type == TokenType::Identifier)
			{
				t += 2;
			}
			else
			{
				// Unexpected token
			}
		}
		else
		{
			// Unexpected token
		}
	}
}