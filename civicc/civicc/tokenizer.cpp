#include <cctype>
#include <algorithm>
#include <sstream>

#include "tokenizer.h"

const char* Tokenizer::identifierPattern = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_";
const char* Tokenizer::numberPattern = "1234567890";

Tokenizer::Tokenizer(std::istream& istream) :
	lineNumber(0),
	linePos(0),
	line(),
	istream(istream)
{
}

bool Tokenizer::GetNextToken(Token& token)
{
	if(!istream.good()) return false;

	bool ignore = false;

	for(;;)
	{
		linePos = line.find_first_not_of(" \t", linePos);

		if(linePos >= line.length())
		{
			if(!istream.good()) return false;

			linePos = 0;
			lineNumber++;
			std::getline(istream, line);
		}

		if(ignore)
		{
			linePos = line.find("*/", linePos);
			if(linePos != std::string::npos)
			{
				linePos += 2;
				ignore = false;
			}
		}
		else if(line.find("//", linePos, 2) == linePos)
		{
			linePos = std::string::npos;
		}
		else if(line.find("/*", linePos, 2) == linePos)
		{
			ignore = true;
			linePos += 2;
		}
		else
		{
			break;
		}
	}

	token.type = TokenType::Unknown;
	token.line = lineNumber;
	token.pos = linePos;
		
	const char first = std::toupper(line[linePos]);

	if(first >= 'A' && first <= 'Z') TokenizeWord(token);
	else if(first >= '0' && first <= '9' || first == '.') TokenizeNumber(token);
	else TokenizeSymbol(token);

	return true;
}

void Tokenizer::TokenizeWord(Token& token)
{
	auto uppercase = line;
	std::transform(uppercase.begin(), uppercase.end(), uppercase.begin(), std::toupper);
	const size_t end = uppercase.find_first_not_of(identifierPattern, linePos);
	token.readString = line.substr(linePos, end - linePos);

	auto reservedWord = GetReservedWord(token.readString);
	switch(reservedWord)
	{
	case ReservedWord::True:
		token.type = TokenType::BoolType;
		token.boolValue = true;
		break;

	case ReservedWord::False:
		token.type = TokenType::BoolType;
		token.boolValue = false;
		break;

	case ReservedWord::Undefined:
		token.type = TokenType::Identifier;
		break;

	default:
		token.type = TokenType::ReservedWord;
		token.reservedWord = reservedWord;
	}

	linePos = end;
}

void Tokenizer::TokenizeNumber(Token& token)
{
	size_t end = line.find_first_not_of(numberPattern, linePos);
	std::stringstream sstream;

	if(line[end] == '.')
	{
		end = line.find_first_not_of(numberPattern, end + 1);
		token.readString = line.substr(linePos, end - linePos);
		token.type = TokenType::FloatType;
		sstream.str(token.readString);
		sstream >> token.floatValue;
	}
	else
	{
		token.readString = line.substr(linePos, end - linePos);
		token.type = TokenType::IntType;
		sstream.str(token.readString);
		sstream >> token.intValue;
	}

	linePos = end;
}

void Tokenizer::TokenizeSymbol(Token& token)
{
	auto reservedSymbol = GetReservedSymbol(line, linePos);
	
	switch(reservedSymbol)
	{
	case ReservedSymbol::Equals:
	case ReservedSymbol::Unequals:
	case ReservedSymbol::LessEqual:
	case ReservedSymbol::MoreEqual:
	case ReservedSymbol::And:
	case ReservedSymbol::Or:
		token.type = TokenType::ReservedSymbol;
		token.readString = line.substr(linePos, 2);
		token.reservedSymbol = reservedSymbol;
		linePos += 2;
		break;

	case ReservedSymbol::Undefined:
		token.readString = line[linePos];
		linePos++;
		break;

	default:
		token.type = TokenType::ReservedSymbol;
		token.readString = line[linePos];
		token.reservedSymbol = reservedSymbol;
		linePos++;
	}
}