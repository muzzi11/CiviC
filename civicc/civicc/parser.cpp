#include <sstream>
#include <iostream>

#include "parser.h"


ParseException::ParseException(const std::string& str)
{
	msg = str + "\n";
}

ParseException::ParseException(const std::string& str, const Token& token)
{
	std::stringstream sstream(str);
	sstream << str << " at line " << token.line << " column " << token.pos + 1 << "\n";
	msg = sstream.str();
}

const char* ParseException::what() const
{
	return msg.c_str();
}

Parser::Parser(const std::vector<Token>& tokens) : 
	t(0),
	tokens(tokens)
{
}

void Parser::ParseProgram(Node::NodePtr node)
{
	root = node;
	while(t < tokens.size())
	{
		if(!Declaration())
		{
			throw ParseException("Unexpected token", tokens[t]);
		}
	}
}

void Parser::CheckUnexpectedEOF() const
{
	if(t >= tokens.size()) throw ParseException("Unexpected end of file.");
}

bool IsType(const Token& t)
{
	return t == ReservedWord::Bool || t == ReservedWord::Int || t == ReservedWord::Float || t == ReservedWord::Void;
}

Node::Type ReservedWordToType(ReservedWord word)
{
	if(word == ReservedWord::Bool) return Node::Type::Bool;
	else if(word == ReservedWord::Int) return Node::Type::Int;
	else if(word == ReservedWord::Float) return Node::Type::Float;
	else return Node::Type::Void;
}

int ExtractVariable(int start, const std::vector<Token>& stack, Node::Variable& out)
{
	int i;
	for(i = start; i < stack.size(); ++i)
	{
		if(!IsType(stack[i])) continue;

		out.type = ReservedWordToType(stack[i++].reservedWord);
		if(stack[i] == ReservedSymbol::BracketL)
		{
			for(; stack[i] != ReservedSymbol::BracketR; ++i)
			{
				if(stack[i].type == TokenType::IntType) out.dim.push_back(stack[i].intValue);
			}
		}

		out.name = stack[++i].readString;
		return i;
	}

	return -1;
}

int ExtractFunctionHeader(int start, const std::vector<Token>& stack, Node::FunctionHeader& out)
{
	int i;
	out.returnType = ReservedWordToType(stack[start].reservedWord);
	out.name = stack[start + 1].readString;

	for(i = start + 2; stack[i] != ReservedSymbol::ParenthesesR; ++i)
	{
		if(stack[i].type != TokenType::ReservedWord) continue;

		Node::Variable var;
		i = ExtractVariable(i, stack, var);
		if(i >= 0) out.args.push_back(var);
	}

	return  ++i;
}

void Parser::AddFunctionDec()
{
	auto node = std::make_shared<Node::FunctionDec>();
	ExtractFunctionHeader(0, stack, node->header);
	stack.clear();
	root->children.push_back(node);
}

void Parser::AddGlobalDec()
{
	auto node = std::make_shared<Node::GlobalDec>();
	ExtractVariable(1, stack, node->var);
	stack.clear();
	root->children.push_back(node);
}

void Parser::AddFunctionDef()
{
	auto node = std::make_shared<Node::FunctionDef>();
	node->export = stack[0] == ReservedWord::Export;
	ExtractFunctionHeader(node->export ? 1 : 0, stack, node->header);	
	stack.clear();
	if(scopes.empty()) root->children.push_back(node);
	else scopes.back()->children.push_back(node);
	scopes.push_back(node);
}

void Parser::AddGlobalDef()
{
	auto node = std::make_shared<Node::GlobalDef>();
	node->export = stack[0] == ReservedWord::Export;
	ExtractVariable(node->export ? 1 : 0, stack, node->var);
	stack.clear();
	root->children.push_back(node);
	scopes.push_back(node);
}

void Parser::AddReturn()
{
	auto node = std::make_shared<Node::Return>();
	stack.clear();
	scopes.back()->children.push_back(node);
	scopes.push_back(node);
}

void Parser::AddVarDec()
{
	auto node = std::make_shared<Node::VarDec>();
	ExtractVariable(0, stack, node->var);
	stack.clear();
	scopes.back()->children.push_back(node);
	scopes.push_back(node);
}

void Parser::AddAssignment()
{
	auto node = std::make_shared<Node::Assignment>();
	node->name = stack[0].readString;
	stack.clear();
	scopes.back()->children.push_back(node);
	scopes.push_back(node);
}

bool Parser::Declaration()
{
	return Extern() && Dec() || Export() && Def();
}

bool Parser::Dec()
{
	return Type() && ArrayId() && Id(true) && ((FunHeader() && FunDec()) || GlobalDec()) ||
		Void() && Id(true) && FunHeader(true) && FunDec();
}

bool Parser::FunHeader(bool error)
{
	return ParenthesesL(error) && Param() && Params() && ParenthesesR();
}

bool Parser::Param()
{
	return Type() && ArrayId() && Id(true) || true;
}

bool Parser::Params()
{
	return Comma() && Type(true) && ArrayId() && Id(true) && Params() || true;
}

bool Parser::FunDec()
{
	Semicolon();
	AddFunctionDec();
	return true;
}

bool Parser::GlobalDec()
{
	Semicolon();
	AddGlobalDec();
	return true;
}

bool Parser::Def()
{
	if(Void() && Id(true) && FunHeader(true))
	{
		AddFunctionDef();
		return FunDef();
	}
	else if(Type())
	{
		if(ArrayExpr() && Id(true) && AssignOpt() && GlobalDef())
		{
			return true;
		}
		else if(Id())
		{
			if(FunHeader())
			{
				AddFunctionDef();
				return FunDef();
			}
			
			return AssignOpt() && GlobalDef();
		}
	}

	return false;
}

bool Parser::Export()
{
	if(Word(ReservedWord::Export)) infoPool.bools.push_back(true);
	return true;
}

bool Parser::GlobalDef()
{
	Semicolon();
	stack.clear();
	scopes.pop_back();
	return true;
}

bool Parser::FunDef()
{
	BraceL(true) && FunBody() && BraceR();
	stack.clear();
	scopes.pop_back();
	return true;
}

bool Parser::Type(bool error)
{
	if(Word(ReservedWord::Bool))
	{
		infoPool.types.push_back(Node::Type::Bool);
		return true;
	}
	else if(Word(ReservedWord::Int))
	{
		infoPool.types.push_back(Node::Type::Int);
		return true;
	}
	else if(Word(ReservedWord::Float))
	{
		infoPool.types.push_back(Node::Type::Float);
		return true;
	}

	if(error) throw ParseException("Missing type", tokens[t]);

	return false;
}

bool Parser::FunBody()
{
	return Locals() && Statements() && Return();
}

bool Parser::Locals()
{
	if(Void() && Id(true) && LocalFun(true) && LocalFuns())
	{
		return true;
	}
	else if(Type())
	{
		if(ArrayExpr())
		{
			Id(true); AddVarDec(); AssignOpt(); Semicolon();
			stack.clear();
			scopes.pop_back();
			return Locals();
		}
		else if(Id())
		{
			if(LocalFun() && LocalFuns()) return true;

			AddVarDec();
			AssignOpt(); Semicolon();
			stack.clear();
			scopes.pop_back();
			return Locals();
		}
		else
		{
			throw ParseException("Expected a function definition or variable declaration", tokens[t - 1]);
		}
	}
	
	return true;
}

bool Parser::LocalFun(bool error)
{
	if(FunHeader(error))
	{
		AddFunctionDef();
		return BraceL(true) && FunBody() && BraceR();
	}

	return false;
}

bool Parser::LocalFuns()
{
	if(Void() && Id(true) && LocalFun(true) && LocalFuns())
	{
		return true;
	}
	else if(Type())
	{
		if(ArrayExpr()) throw ParseException("Unexpected array expression(variable declarations should preceed function definitions)", tokens[t - 1]);
		else if(Id(true))
		{
			if(AssignOpt() || Semicolon()) throw ParseException("Variable declaration should preceed function definitions", tokens[t - 1]);
			return LocalFun(true) && LocalFuns();
		}
	}

	return true;
}

bool Parser::ArrayExpr()
{
	return BracketL() && Expr() && Exprs() && BracketR();
}

bool Parser::ArrayId()
{
	return BracketL() && Id(true) && Ids() && BracketR() || true;
}

bool Parser::Ids()
{
	return Comma() && Id(true) && Ids() || true;
}

bool Parser::Statement()
{
	if(Id())
	{
		if(ArrayExpr() && Assign() && Semicolon() ||
			Assign() && Semicolon() ||
			ParenthesesL() && (ParenthesesR() && Semicolon() || Expr() && Exprs() && ParenthesesR() && Semicolon()))
		{
			return true;
		}
		else if(ArrayExpr())
		else
		{
			throw ParseException("Invalid statement, expected an assignment or function call after identifier", tokens[t]);
		}
	}
	return If() && ParenthesesL(true) && Expr() && ParenthesesR() && Block() && ElseBlock() ||
		While() && ParenthesesL(true) && Expr() && ParenthesesR() && Block() ||
		Do() && Block() && While(true) && ParenthesesL(true) && Expr() && ParenthesesR() && Semicolon() ||
		For() && ParenthesesL(true) && Int() && Id(true) && Assign() && Comma(true) && Expr() && Step() && ParenthesesR() && Block();
}

bool Parser::Statements()
{
	return Statement() && Statements() || true;
}

bool Parser::Step()
{
	return Comma() && Expr() || true;
}

bool Parser::Block()
{
	return BraceL() && Statements() && BraceR() || Statement();
}

bool Parser::ElseBlock()
{
	return Else() && Block() || true;
}

bool Parser::Return()
{
	if(Word(ReservedWord::Return))
	{
		AddReturn();
		Expr(); Semicolon();
		stack.clear();
		scopes.pop_back();
	}
	
	return true;
}

bool Parser::Assign()
{
	return Symbol(ReservedSymbol::Assign) && Expr();
}

bool Parser::AssignOpt()
{
	return Assign() || true;
}

bool Parser::Expr()
{
	auto node = Expr(1);
	if(!node) throw ParseException("Invalid expression", tokens[t]);
	return true;
}

NodePtr Parser::Expr(int precedence)
{
	auto left = P();

	while(left != nullptr && BinaryOp() && Precedence(t) >= precedence)
	{
		int q = RightAssociative(t) ? Precedence(t) : Precedence(t) + 1;
		const Token& token = tokens[t++];

		auto right = Expr(q);
		auto op = std::make_shared<BinaryOpNode>(token);
		op->children.push_back(left);
		op->children.push_back(right);
		
		left = op;
	}

	return left;
}

NodePtr Parser::P()
{
	if(UnaryOp())
	{
		int q = Precedence(t);
		const Token& token = tokens[t++];
		auto op = std::make_shared<UnaryOpNode>(token);
		op->children.push_back(Expr(q));
		return op;
	}
	else if(ParenthesesL())
	{
		auto expr = Expr(1);
		ParenthesesR();
		return expr;
	}
	else if(Literal())
	{
		const Token& token = tokens[t++];
		return std::make_shared<LiteralNode>(token);
	}
	else if(Id())
	{
		if(ParenthesesL())
		{
			auto call = std::make_shared<CallNode>(tokens[t-2]);
			
			bool comma = false;
			do
			{
				auto expr = Expr(1);
				if(expr != nullptr) call->children.push_back(expr);
				else if(comma) throw ParseException("Expected an expression after ','", tokens[t - 1]);
				comma = true;
			} while(Comma());

			ParenthesesR();

			return call;
		}

		const Token& id = tokens[t - 1];
		return std::make_shared<IdentifierNode>(id);
	}
	
	return nullptr;
}

bool Parser::Literal() const
{
	if(t >= tokens.size()) return false;

	const Token& tok = tokens[t];
	return tok.type == TokenType::BoolType || tok.type == TokenType::IntType || tok.type == TokenType::FloatType;
}

bool Parser::BinaryOp() const
{
	if(t >= tokens.size()) return false;

	int p = Precedence(t);
	return p > 0 && p < unaryPrecedence;
}

bool Parser::UnaryOp() const
{
	if(t >= tokens.size()) return false;
	return Precedence(t) == unaryPrecedence;
}

bool Parser::Exprs()
{
	return Comma() && Expr() && Exprs() || true;
}

int Parser::Precedence(size_t tokenIndex) const
{
	if(tokenIndex == 0) return 0;

	const Token& token = tokens[tokenIndex];
	const int addPresedence = 3, minusPresedence = addPresedence;

	if(token == ReservedSymbol::And || token == ReservedSymbol::Or) return 1;
	else if(token == ReservedSymbol::Equals || token == ReservedSymbol::Unequals || token == ReservedSymbol::Less ||
		token == ReservedSymbol::LessEqual || token == ReservedSymbol::More || token == ReservedSymbol::MoreEqual) return 2;
	else if(token == ReservedSymbol::Plus) return addPresedence;
	else if(token == ReservedSymbol::Multiply || token == ReservedSymbol::Divide || token == ReservedSymbol::Modulo) return 4;
	else if(token == ReservedSymbol::Not) return unaryPrecedence;
	else if(token == ReservedSymbol::Minus)
	{
		const Token& prev = tokens[tokenIndex - 1];

		if(prev.type == TokenType::BoolType || prev.type == TokenType::IntType || prev.type == TokenType::FloatType ||
			prev.type == TokenType::Identifier || prev == ReservedSymbol::ParenthesesR || prev == ReservedSymbol::BracketR)
		{
			return minusPresedence;
		}
		else
		{
			return unaryPrecedence;
		}
	}

	return 0;
}

bool Parser::RightAssociative(size_t tokenIndex) const
{
	const Token& token = tokens[tokenIndex];

	if(token == ReservedSymbol::Not) return true;
	else if(token == ReservedSymbol::Minus) return Precedence(tokenIndex) == unaryPrecedence;

	return false;
}


bool Parser::Word(ReservedWord word, bool eofError)
{
	if(eofError) CheckUnexpectedEOF();
	if(t >= tokens.size()) return false;

	if(tokens[t] == word)
	{
		stack.push_back(tokens[t]);
		t++;
		return true;
	}

	return false;
}

bool Parser::Symbol(ReservedSymbol symbol, bool eofError)
{
	if(eofError) CheckUnexpectedEOF();
	if(t >= tokens.size()) return false;

	if(tokens[t] == symbol)
	{
		stack.push_back(tokens[t]);
		t++;
		return true;
	}

	return false;
}

bool Parser::Id(bool error)
{
	CheckUnexpectedEOF();

	if(tokens[t].type == TokenType::Identifier)
	{
		infoPool.strings.push_back(tokens[t].readString);
		stack.push_back(tokens[t]);
		t++;
		return true;
	}

	if(error) throw ParseException("Missing identifier", tokens[t]);

	return false;
}

bool Parser::Void()
{
	if(Word(ReservedWord::Void))
	{
		infoPool.types.push_back(Node::Type::Void);
		return true;
	}

	return false;
}

bool Parser::ParenthesesL(bool error)
{
	if(!Symbol(ReservedSymbol::ParenthesesL))
	{
		if(error) throw ParseException("Expected a '(' ", tokens[t]);
		return false;
	}
	return true;
}

bool Parser::ParenthesesR()
{
	if(!Symbol(ReservedSymbol::ParenthesesR)) throw ParseException("Expected a ')' ", tokens[t]);
	return true;
}

bool Parser::BraceL(bool error)
{
	if(!Symbol(ReservedSymbol::BraceL))
	{
		if(error) throw ParseException("Expected a '{' ", tokens[t]);
		return false;
	}
	return true;
}

bool Parser::BraceR()
{
	if(!Symbol(ReservedSymbol::BraceR, false)) throw ParseException("Expected a '}' ", tokens[t - 1]);
	return true;
}

bool Parser::BracketL(bool error)
{
	if(!Symbol(ReservedSymbol::BracketL))
	{
		if(error) throw ParseException("Expected a '[' ", tokens[t]);
		return false;
	}
	return true;
}

bool Parser::BracketR()
{
	if(!Symbol(ReservedSymbol::BracketR)) throw ParseException("Expected a ']' ", tokens[t]);
	return true;
}

bool Parser::Comma(bool error)
{
	if(!Symbol(ReservedSymbol::Comma))
	{
		if(error) throw ParseException("Expected a ',' ", tokens[t]);
		return false;
	}
	return true;
}

bool Parser::Semicolon()
{
	if(!Symbol(ReservedSymbol::Semicolon, false)) throw ParseException("Expected a ';' ", tokens[t - 1]);
	return true;
}

bool Parser::Extern()
{
	return Word(ReservedWord::Extern);
}

bool Parser::If()
{
	return Word(ReservedWord::If);
}

bool Parser::Else()
{
	return Word(ReservedWord::Else);
}

bool Parser::While(bool error)
{
	if(!Word(ReservedWord::While))
	{
		if(error) throw ParseException("Expected the keyword 'while'", tokens[t]);
		return false;
	}
	return true;
}

bool Parser::Do()
{
	return Word(ReservedWord::Do);
}

bool Parser::For()
{
	return Word(ReservedWord::For);
}

bool Parser::Int()
{
	if(!Word(ReservedWord::Int)) throw ParseException("Expected type 'int'", tokens[t]);
	return true;
}