#include <istream>

#include "token.h"


class Tokenizer
{
public:
	Tokenizer(std::istream& istream);
	Tokenizer(const Tokenizer&) = delete;
	Tokenizer(Tokenizer&&) = delete;
	Tokenizer& operator=(const Tokenizer&) = delete;
	Tokenizer& operator=(Tokenizer&&) = delete;

	bool GetNextToken(Token& token);

private:
	size_t lineNumber, linePos;
	std::string line;
	std::istream& istream;

	static const char* identifierPattern;
	static const char* numberPattern;

	void TokenizeWord(Token& token);
	void TokenizeNumber(Token& token);
	void TokenizeSymbol(Token& token);
};