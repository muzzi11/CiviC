#include <istream>

#include "token.h"


class Tokenizer
{
private:
	size_t lineNumber, linePos;
	std::string line;
	std::istream& istream;

	static const char* identifierPattern;
	static const char* numberPattern;

	void TokenizeWord(Token& token);
	void TokenizeNumber(Token& token);
	void TokenizeSymbol(Token& token);

public:
	Tokenizer(std::istream& istream);

	bool GetNextToken(Token& token);
};