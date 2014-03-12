#pragma once

#include <vector>

#include "token.h"


class Parser
{
public:
	Parser(const std::vector<Token>& tokens);

	void ParseProgram();

private:
	size_t t;
	const int unaryPresedence = 500;
	const std::vector<Token>& tokens;
	
	bool Declaration();
	bool Dec();
	bool FunHeader();
	bool Param();
	bool Params();
	bool FunDec();
	bool GlobalDec();
	bool Def();
	bool Export();
	bool GlobalDef();
	bool FunDef();
	bool Type();
	bool FunBody();
	bool LocalFunDef();
	bool VarDecs();
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
	bool Expr();
	bool Exprs();
	bool Args();

	int Presedence(size_t tokenIndex);
	bool RightAssociative(size_t tokenIndex);

	bool Word(ReservedWord word);
	bool Symbol(ReservedSymbol symbol);

	bool Id();
	bool Void();
	bool ParenthesesL();
	bool ParenthesesR();
	bool BraceL();
	bool BraceR();
	bool BracketL();
	bool BracketR();
	bool Comma();
	bool Semicolon();
	bool Extern();
	bool If();
	bool Else();
	bool While();
	bool Do();
	bool For();
	bool Int();
};