#pragma once

#include <string>


enum class TokenType
{
	Unknown,
	ReservedWord,
	ReservedSymbol,
	Identifier,
	BoolType,
	IntType,
	FloatType
};

enum class ReservedWord
{
	Undefined,
	If,
	Then,
	Else,
	For,
	Do,
	While,
	Return,
	Export,
	Extern,
	True,
	False,
	Bool,
	Int,
	Float,
	Void
};

enum class ReservedSymbol
{
	Undefined,
	Equals,
	Unequals,
	LessEqual,
	MoreEqual,
	Less,
	More,
	And,
	Or,
	Not,
	ParenthesesL,
	ParenthesesR,
	BracketL,
	BracketR,
	BraceL,
	BraceR,
	Assign,
	Plus,
	Minus,
	Multiply,
	Divide,
	Modulo,
	Semicolon
};

ReservedWord GetReservedWord(const std::string& str);
ReservedSymbol GetReservedSymbol(const std::string& str, size_t pos);

struct Token
{
	TokenType type;
	int line, pos;
	std::string readString;

	union
	{
		bool boolValue;
		int intValue;
		float floatValue;
	};

	union
	{
		ReservedWord reservedWord;
		ReservedSymbol reservedSymbol;
	};
};