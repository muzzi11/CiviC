#pragma once

#include <vector>
#include <string>

#include "token.h"
#include "node.h"


class ParseException : public std::exception
{
public:
	ParseException(const std::string& msg);
	ParseException(const std::string& msg, const Token& token);
	const char* what() const throw() override;

private:
	std::string msg;
};


class Parser
{
public:
	Parser(const std::vector<Token>& tokens);

	void ParseProgram(Nodes::NodePtr root);

private:
	size_t t;
	const std::vector<Token>& tokens;

	Nodes::NodePtr root;
	std::vector<Nodes::NodePtr> scopes;
	std::vector<Token> stack;
	
	void CheckUnexpectedEOF() const;

	void AddFunctionDec();
	void AddGlobalDec();
	void AddFunctionDef();
	void AddGlobalDef();
	void AddReturn();
	void AddVarDec();
	void AddAssignment();
	std::shared_ptr<Nodes::Call> AddCall();
	void AddIf();
	void AddElse();
	void AddWhile();
	void AddDoWhile();
	void AddFor();

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
	bool Expr();
	Nodes::NodePtr Expr(int precedence);
	Nodes::NodePtr P();
	bool Literal() const;
	bool BinaryOp() const;
	bool UnaryOp() const;
	bool Exprs();
	bool Args();

	int Precedence(size_t tokenIndex) const;
	bool RightAssociative(size_t tokenIndex) const;

	bool Word(ReservedWord word, bool eofError = true);
	bool Symbol(ReservedSymbol symbol, bool eofError = true);

	bool Id(bool error = false);
	bool Void();
	bool ParenthesesL(bool error = false);
	bool ParenthesesR(bool error = true);
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
