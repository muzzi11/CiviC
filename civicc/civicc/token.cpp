#include <unordered_map>

#include "token.h"


Token::Token(ReservedWord word) :
	type(TokenType::ReservedWord),
	reservedWord(word)
{
}

Token::Token(ReservedSymbol symbol) :
	type(TokenType::ReservedSymbol),
	reservedSymbol(symbol)
{
}

bool Token::operator==(ReservedWord word) const
{
	return type == TokenType::ReservedWord && reservedWord == word;
}

bool Token::operator!=(ReservedWord word) const
{
	return !operator==(word);
}

bool Token::operator==(ReservedSymbol symbol) const
{
	return type == TokenType::ReservedSymbol && reservedSymbol == symbol;
}

bool Token::operator!=(ReservedSymbol symbol) const
{
	return !operator==(symbol);
}

ReservedWord GetReservedWord(const std::string& str)
{
	static const std::unordered_map<std::string, ReservedWord> map(
	{
		{ "if", ReservedWord::If },
		{ "then", ReservedWord::Then },
		{ "else", ReservedWord::Else },
		{ "for", ReservedWord::For },
		{ "do", ReservedWord::Do },
		{ "while", ReservedWord::While },
		{ "return", ReservedWord::Return },
		{ "export", ReservedWord::Export },
		{ "extern", ReservedWord::Extern },
		{ "true", ReservedWord::True },
		{ "false", ReservedWord::False },
		{ "bool", ReservedWord::Bool },
		{ "int", ReservedWord::Int },
		{ "float", ReservedWord::Float },
		{ "void", ReservedWord::Void },
	});

	return (map.count(str) > 0) ? map.at(str) : ReservedWord::Undefined;
}

ReservedSymbol GetReservedSymbol(const std::string& str, size_t pos)
{
	static const std::unordered_map<std::string, ReservedSymbol> map(
	{
		{ "==", ReservedSymbol::Equals },
		{ "!=", ReservedSymbol::Unequals },
		{ "<=", ReservedSymbol::LessEqual },
		{ ">=", ReservedSymbol::MoreEqual },
		{ "<", ReservedSymbol::Less },
		{ ">", ReservedSymbol::More },
		{ "&&", ReservedSymbol::And },
		{ "||", ReservedSymbol::Or },
		{ "!", ReservedSymbol::Not },
		{ "(", ReservedSymbol::ParenthesesL },
		{ ")", ReservedSymbol::ParenthesesR },
		{ "[", ReservedSymbol::BracketL },
		{ "]", ReservedSymbol::BracketR },
		{ "{", ReservedSymbol::BraceL },
		{ "}", ReservedSymbol::BraceR },
		{ "=", ReservedSymbol::Assign },
		{ "+", ReservedSymbol::Plus },
		{ "-", ReservedSymbol::Minus },
		{ "*", ReservedSymbol::Multiply },
		{ "/", ReservedSymbol::Divide },
		{ "%", ReservedSymbol::Modulo },
		{ ",", ReservedSymbol::Comma },
		{ ";", ReservedSymbol::Semicolon },
	});

	const std::string a = str.substr(pos, 2);
	const std::string b = str.substr(pos, 1);

	if(map.count(a) > 0) return map.at(a);

	return (map.count(b) > 0) ? map.at(b) : ReservedSymbol::Undefined;
}