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
	ReservedWord word;

	if(str.compare("if") == 0) word = ReservedWord::If;
	else if(str.compare("then") == 0) word = ReservedWord::Then;
	else if(str.compare("else") == 0) word = ReservedWord::Else;
	else if(str.compare("for") == 0) word = ReservedWord::For;
	else if(str.compare("do") == 0) word = ReservedWord::Do;
	else if(str.compare("while") == 0) word = ReservedWord::While;
	else if(str.compare("return") == 0) word = ReservedWord::Return;
	else if(str.compare("export") == 0) word = ReservedWord::Export;
	else if(str.compare("extern") == 0) word = ReservedWord::Extern;
	else if(str.compare("true") == 0) word = ReservedWord::True;
	else if(str.compare("false") == 0) word = ReservedWord::False;
	else if(str.compare("bool") == 0) word = ReservedWord::Bool;
	else if(str.compare("int") == 0) word = ReservedWord::Int;
	else if(str.compare("float") == 0) word = ReservedWord::Float;
	else if(str.compare("void") == 0) word = ReservedWord::Void;
	else word = ReservedWord::Undefined;

	return word;
}

ReservedSymbol GetReservedSymbol(const std::string& str, size_t pos)
{
	ReservedSymbol symbol;

	if(str.compare(pos, 2, "==") == 0) symbol = ReservedSymbol::Equals;
	else if(str.compare(pos, 2, "!=") == 0) symbol = ReservedSymbol::Unequals;
	else if(str.compare(pos, 2, "<=") == 0) symbol = ReservedSymbol::LessEqual;
	else if(str.compare(pos, 2, ">=") == 0) symbol = ReservedSymbol::MoreEqual;
	else if(str.compare(pos, 1, "<") == 0) symbol = ReservedSymbol::Less;
	else if(str.compare(pos, 1, ">") == 0) symbol = ReservedSymbol::More;
	else if(str.compare(pos, 2, "&&") == 0) symbol = ReservedSymbol::And;
	else if(str.compare(pos, 2, "||") == 0) symbol = ReservedSymbol::Or;
	else if(str.compare(pos, 1, "!") == 0) symbol = ReservedSymbol::Not;
	else if(str.compare(pos, 1, "(") == 0) symbol = ReservedSymbol::ParenthesesL;
	else if(str.compare(pos, 1, ")") == 0) symbol = ReservedSymbol::ParenthesesR;
	else if(str.compare(pos, 1, "[") == 0) symbol = ReservedSymbol::BracketL;
	else if(str.compare(pos, 1, "]") == 0) symbol = ReservedSymbol::BracketR;
	else if(str.compare(pos, 1, "{") == 0) symbol = ReservedSymbol::BraceL;
	else if(str.compare(pos, 1, "}") == 0) symbol = ReservedSymbol::BraceR;
	else if(str.compare(pos, 1, "=") == 0) symbol = ReservedSymbol::Assign;
	else if(str.compare(pos, 1, "+") == 0) symbol = ReservedSymbol::Plus;
	else if(str.compare(pos, 1, "-") == 0) symbol = ReservedSymbol::Minus;
	else if(str.compare(pos, 1, "*") == 0) symbol = ReservedSymbol::Multiply;
	else if(str.compare(pos, 1, "/") == 0) symbol = ReservedSymbol::Divide;
	else if(str.compare(pos, 1, "%") == 0) symbol = ReservedSymbol::Modulo;
	else if(str.compare(pos, 1, ",") == 0) symbol = ReservedSymbol::Comma;
	else if(str.compare(pos, 1, ";") == 0) symbol = ReservedSymbol::Semicolon;
	else symbol = ReservedSymbol::Undefined;

	return symbol;
}