#pragma once

#include <vector>
#include <string>

#include "token.h"


class ParseException : public std::exception
{
public:
	ParseException(const std::string& msg, const Token& token);
	const char* what() const override;

private:
	std::string msg;
};

class Parser
{
public:
	Parser(const std::vector<Token>& tokens);

	void ParseProgram();

private:
	size_t t;
	const unsigned int unaryPresedence = 500;
	const std::vector<Token>& tokens;
	
	bool Declaration();
	bool Dec();
	bool FunHeader(bool error = false);
	bool Param();
	bool Params();
	bool FunDec();
	bool GlobalDec();
	bool Def();
	bool Export();
	bool GlobalDef();
	bool FunDef();
	bool Type(bool error = false);
	bool FunBody();
	bool Locals();
	bool LocalFun(bool error = false);
	bool LocalFuns();
	bool ArrayExpr();
	bool ArrayId();
	bool Ids();
	bool Statement();
	bool Statements();
	bool Step();
	bool Block();
	bool ElseBlock();
	bool Return();
	bool Assign();
	bool AssignOpt();
	bool Expr(bool error = false);
	bool Exprs();
	bool Args();

	unsigned int Presedence(size_t tokenIndex);
	bool RightAssociative(size_t tokenIndex);

	bool Word(ReservedWord word);
	bool Symbol(ReservedSymbol symbol);

	bool Id(bool error = false);
	bool Void();
	bool ParenthesesL(bool error = false);
	bool ParenthesesR();
	bool BraceL(bool error = false);
	bool BraceR();
	bool BracketL(bool error = false);
	bool BracketR();
	bool Comma(bool error = false);
	bool Semicolon();
	bool Extern();
	bool If();
	bool Else();
	bool While(bool error = false);
	bool Do();
	bool For();
	bool Int();
};