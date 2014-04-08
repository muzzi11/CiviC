#include <sstream>
#include <iostream>
#include <map>

#include "parser.h"


const int unaryPrecedence = 5;

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

const char* ParseException::what() const throw()
{
	return msg.c_str();
}

Parser::Parser(const std::vector<Token>& tokens) : 
	t(0),
	tokens(tokens)
{
}

void Parser::ParseProgram(Nodes::NodePtr node)
{
	root = node;
	while(t < tokens.size())
	{
		if(!Declaration())
		{
			throw ParseException("Invalid declaration", tokens[t]);
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

Nodes::Type TokenToType(const Token& t)
{
	if(t == ReservedWord::Bool) return Nodes::Type::Bool;
	else if(t == ReservedWord::Int) return Nodes::Type::Int;
	else if(t == ReservedWord::Float) return Nodes::Type::Float;
	else if(t == ReservedWord::Void) return Nodes::Type::Void;
	else return Nodes::Type::None;
}

void ExtractParameters(int start, const std::vector<Token>& stack, std::vector<Nodes::Param>& out)
{
	for(size_t i = start; i < stack.size(); ++i)
	{
		if(!IsType(stack[i])) continue;

		out.emplace_back();
		out.back().type = TokenToType(stack[i]);

		if(stack[++i] == ReservedSymbol::BracketL)
		{
			for(; stack[i] != ReservedSymbol::BracketR; ++i)
			{
				if(stack[i].type == TokenType::Identifier) out.back().dim.push_back(stack[i].readString);
			}
			++i;
		}

		out.back().name = stack[i].readString;
		out.back().pos = stack[i].pos;
		out.back().line = stack[i].line;
	}
}

void Parser::AddFunctionDec()
{
	auto node = std::make_shared<Nodes::FunctionDec>();

	node->header.returnType = TokenToType(stack[0]);
	node->header.name = stack[1].readString;
	node->line = stack[1].line;
	node->pos = stack[1].pos;
	ExtractParameters(2, stack, node->header.params);

	root->children.push_back(node);
	stack.clear();
}

void Parser::AddGlobalDec()
{
	auto node = std::make_shared<Nodes::GlobalDec>();

	std::vector<Nodes::Param> temp;
	ExtractParameters(0, stack, temp);
	node->param = temp[0];

	root->children.push_back(node);
	stack.clear();
}

void Parser::AddFunctionDef()
{
	auto node = std::make_shared<Nodes::FunctionDef>();

	node->exp = stack[0] == ReservedWord::Export;
	int i = node->exp ? 1 : 0;
	node->header.returnType = TokenToType(stack[i]);
	node->header.name = stack[i + 1].readString;
	node->pos = stack[i + 1].pos;
	node->line = stack[i + 1].line;
	ExtractParameters(i + 2, stack, node->header.params);

	if(scopes.empty()) root->children.push_back(node);
	else scopes.back()->children.push_back(node);
	scopes.push_back(node);
	stack.clear();
}

void Parser::AddGlobalDef()
{
	auto node = std::make_shared<Nodes::GlobalDef>();
	
	node->exp = stack[0] == ReservedWord::Export;
	int i = node->exp ? 1 : 0;
	node->var.type = TokenToType(stack[i]);
	node->var.name = stack[i + 1].readString;
	node->pos = stack[i + 1].pos;
	node->line = stack[i + 1].line;

	if(!scopes.empty() && scopes.back()->Family() == Nodes::ArrayExpr::Family())
	{
		node->var.array = true;
		node->children.push_back(scopes.back());
		scopes.pop_back();
	}
	
	root->children.push_back(node);
	scopes.push_back(node);
	stack.clear();
}

void Parser::AddReturn()
{
	auto node = std::make_shared<Nodes::Return>();
	auto funDef = std::static_pointer_cast<Nodes::FunctionDef>(scopes.back());
	node->functionName = funDef->header.name;
	scopes.back()->children.push_back(node);
	scopes.push_back(node);
}

void Parser::AddVarDec()
{
	auto node = std::make_shared<Nodes::VarDec>();

	node->var.type = TokenToType(stack[0]);
	node->var.name = stack[1].readString;
	node->pos = stack[1].pos;
	node->line = stack[1].line;

	if(scopes.back()->Family() == Nodes::ArrayExpr::Family())
	{
		node->var.array = true;
		node->children.push_back(scopes.back());
		scopes.pop_back();
	}

	scopes.back()->children.push_back(node);
	scopes.push_back(node);
	stack.clear();
}

void Parser::AddAssignment()
{
	auto node = std::make_shared<Nodes::Assignment>(stack[0].readString);
	node->pos = stack[0].pos;
	node->line = stack[0].line;

	if(scopes.back()->Family() == Nodes::ArrayExpr::Family())
	{
		node->children.push_back(scopes.back());
		scopes.pop_back();
	}
	
	scopes.back()->children.push_back(node);
	scopes.push_back(node);
	stack.clear();
}

std::shared_ptr<Nodes::Call> Parser::AddCall()
{
	auto node = std::make_shared<Nodes::Call>();
	node->name = stack[0].readString;
	node->pos = stack[0].pos;
	node->line = stack[0].line;
	scopes.back()->children.push_back(node);
	scopes.push_back(node);
	stack.clear();

	return node;
}

void Parser::AddIf()
{
	auto node = std::make_shared<Nodes::If>();
	scopes.back()->children.push_back(node);
	scopes.push_back(node);
}

void Parser::AddElse()
{
	auto node = std::make_shared<Nodes::Else>();
	scopes.back()->children.push_back(node);
	scopes.push_back(node);
}

void Parser::AddWhile()
{
	auto node = std::make_shared<Nodes::While>();
	scopes.back()->children.push_back(node);
	scopes.push_back(node);
}

void Parser::AddDoWhile()
{
	auto node = std::make_shared<Nodes::DoWhile>();
	scopes.back()->children.push_back(node);
	scopes.push_back(node);
}

void Parser::AddFor()
{
	auto node = std::make_shared<Nodes::For>();
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
	AddFunctionDec();
	return Semicolon();
}

bool Parser::GlobalDec()
{
	AddGlobalDec();
	return Semicolon();
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
		if(ArrayExpr() && Id(true))
		{
			AddGlobalDef();
			return AssignOpt() && GlobalDef();
		}
		else if(Id())
		{
			if(FunHeader())
			{
				AddFunctionDef();
				return FunDef();
			}

			AddGlobalDef();
			return AssignOpt() && GlobalDef();
		}
	}

	return false;
}

bool Parser::Export()
{
	if(Word(ReservedWord::Export)) stack.push_back(tokens[t - 1]);
	return true;
}

bool Parser::GlobalDef()
{
	scopes.pop_back();
	return Semicolon();
}

bool Parser::FunDef()
{
	BraceL(true); FunBody();
	if(Type()) throw ParseException("Unexpected variable declaration or function definition", tokens[t - 1]);
	BraceR();
	scopes.pop_back();
	return true;
}

bool Parser::Type(bool error)
{
	if(Word(ReservedWord::Bool) || Word(ReservedWord::Int) || Word(ReservedWord::Float))
	{
		stack.push_back(tokens[t - 1]);
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
			scopes.pop_back();
			return Locals();
		}
		else if(Id())
		{
			if(LocalFun() && LocalFuns()) return true;
			
			AddVarDec();
			AssignOpt(); Semicolon();
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
		BraceL(true); FunBody();
		if(Type()) throw ParseException("Unexpected variable declaration or function definition", tokens[t - 1]);
		BraceR();
		scopes.pop_back();
		return true;
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
		if(ArrayExpr()) throw ParseException("Unexpected array expression(variable declarations should precede function definitions)", tokens[t - 1]);
		else if(Id(true))
		{
			if(LocalFun()) return LocalFuns();
			else if(AssignOpt() || Semicolon()) throw ParseException("Variable declaration should precede function definitions", tokens[t - 1]);
			else(LocalFun(true));
		}
	}

	return true;
}

bool Parser::ArrayExpr()
{
	if(BracketL())
	{
		stack.pop_back();

		auto copy = stack;
		stack.clear();
		scopes.push_back(std::make_shared<Nodes::ArrayExpr>());
		Expr(); Exprs(); BracketR();
		stack.swap(copy);
		return true;
	}
	return false;
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
		if(ArrayExpr())
		{
			AddAssignment();
			Assign(); Semicolon();
			scopes.pop_back();
			return true;
		}
		else if(ParenthesesL())
		{
			AddCall();
			if(ParenthesesR(false) && Semicolon())
			{
				scopes.pop_back();
				return true;
			}
			else
			{
				Expr(); Exprs();
				scopes.pop_back();
				return ParenthesesR() && Semicolon();
			}
		}
		else
		{
			AddAssignment();
			if(Assign() && Semicolon())
			{
				scopes.pop_back();
				return true;
			}
			else throw ParseException("Invalid statement, expected an assignment or function call after identifier", tokens[t]);
		}
	}
	if(If() && ParenthesesL(true) && Expr() && ParenthesesR() && Block() && ElseBlock() ||
		While() && ParenthesesL(true) && Expr() && ParenthesesR() && Block() ||
		Do() && Block() && While(true) && ParenthesesL(true) && Expr() && ParenthesesR() && Semicolon())
	{
		scopes.pop_back();
		return true;
	}
	else if(For() && ParenthesesL(true) && Int() && Id(true))
	{
		AddVarDec();
		if (!Assign()) throw ParseException("Missing initialization of loop counter.", tokens[t]);
		scopes.pop_back(); 
		Comma(true); Expr(); Step(); ParenthesesR(); Block();
		scopes.pop_back();
		return true;
	}

	return false;
}

bool Parser::Statements()
{
	return Statement() && Statements() || true;
}

bool Parser::Step()
{
	if(Comma() && Expr());
	else
	{
		auto literal = std::make_shared<Nodes::Literal>(1);
		literal->pos = tokens[t].pos;
		literal->line = tokens[t].line;
		scopes.back()->children.push_back(literal);
	}
	return true;
}

bool Parser::Block()
{
	if(BraceL())
	{
		Statements(); BraceR();
		//scopes.pop_back();
		return true;
	}
	else if(Statement())
	{
		//scopes.pop_back();
		return true;
	}
	return false;
}

bool Parser::ElseBlock()
{
	if(Else())
	{
		Block();
		scopes.pop_back();
	}
	return true;
}

bool Parser::Return()
{
	if(Word(ReservedWord::Return))
	{
		AddReturn();
		Expr(); Semicolon();
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

Nodes::Operator TokenToBinaryOp(const Token& token)
{
	static const std::map<ReservedSymbol, Nodes::Operator> map(
	{
		{ ReservedSymbol::Plus, Nodes::Operator::Add },
		{ ReservedSymbol::Minus, Nodes::Operator::Subtract },
		{ ReservedSymbol::Multiply, Nodes::Operator::Multiply },
		{ ReservedSymbol::Divide, Nodes::Operator::Divide },
		{ ReservedSymbol::Modulo, Nodes::Operator::Modulo },
		{ ReservedSymbol::Equals, Nodes::Operator::Equal },
		{ ReservedSymbol::Unequals, Nodes::Operator::NotEqual },
		{ ReservedSymbol::Less, Nodes::Operator::Less },
		{ ReservedSymbol::LessEqual, Nodes::Operator::LessEqual },
		{ ReservedSymbol::More, Nodes::Operator::More },
		{ ReservedSymbol::MoreEqual, Nodes::Operator::MoreEqual },
		{ ReservedSymbol::And, Nodes::Operator::And },
		{ ReservedSymbol::Or, Nodes::Operator::Or }
	});

	return map.at(token.reservedSymbol);
}

bool Parser::Expr()
{
	auto node = Expr(1);
	if(!node) throw ParseException("Invalid expression", tokens[t]);
	scopes.back()->children.push_back(node);
	return true;
}

Nodes::NodePtr Parser::Expr(int precedence)
{
	auto left = P();

	while(left != nullptr && BinaryOp() && Precedence(t) >= precedence)
	{
		int q = RightAssociative(t) ? Precedence(t) : Precedence(t) + 1;
		const Token& token = tokens[t++];

		auto right = Expr(q);
		auto op = std::make_shared<Nodes::BinaryOp>(TokenToBinaryOp(token));
		op->children.push_back(left);
		op->children.push_back(right);
		op->pos = token.pos;
		op->line = token.line;
		left = op;
	}

	return left;
}

Nodes::NodePtr Parser::P()
{
	if(UnaryOp())
	{
		int q = Precedence(t);
		const Token& token = tokens[t++];
		Nodes::Operator op = token.reservedSymbol == ReservedSymbol::Not ? Nodes::Operator::Not : Nodes::Operator::Negate;
		auto node = std::make_shared<Nodes::UnaryOp>(op);
		node->children.push_back(Expr(q));
		node->pos = token.pos;
		node->line = token.line;
		return node;
	}
	else if(ParenthesesL())
	{
		if(IsType(tokens[t]))
		{
			const Token& token = tokens[t++];
			if(token == ReservedWord::Void) throw ParseException("Can not cast to 'void'", token);
			ParenthesesR();
			auto node = std::make_shared<Nodes::Cast>(TokenToType(token));
			node->children.push_back(Expr(unaryPrecedence));
			return node;
		}
		auto expr = Expr(1);
		ParenthesesR();
		return expr;
	}
	else if(BracketL())
	{
		stack.clear();

		auto array = std::make_shared<Nodes::ArrayExpr>();

		do
		{
			array->children.push_back(Expr(1));
		} while(Comma());
		
		BracketR();
		stack.clear();
		return array;
	}
	else if(Literal())
	{
		const Token& token = tokens[t++];
		std::shared_ptr<Nodes::Literal> literal;
		if(token.type == TokenType::BoolType) literal = std::make_shared<Nodes::Literal>(token.boolValue);
		else if(token.type == TokenType::IntType) literal = std::make_shared<Nodes::Literal>(token.intValue);
		else literal = std::make_shared<Nodes::Literal>(token.floatValue);
		literal->line = token.line;
		literal->pos = token.pos;
		return literal;
	}
	else if(Id())
	{
		std::string id = stack[0].readString;
		stack.clear();

		if(ParenthesesL())
		{
			auto call = std::make_shared<Nodes::Call>();
			call->name = id;
			call->pos = tokens[t].pos;
			call->line = tokens[t].line;

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
		else if(BracketL())
		{
			auto node = std::make_shared<Nodes::Identifier>(id);
			auto array = std::make_shared<Nodes::ArrayExpr>();
			node->children.push_back(array);
			node->line = tokens[t].line;
			node->pos = tokens[t].pos;

			bool comma = false;
			do
			{
				auto expr = Expr(1);
				if(expr != nullptr) array->children.push_back(expr);
				else if(comma) throw ParseException("Expected an expression after ','", tokens[t - 1]);
				comma = true;
			} while(Comma());

			BracketR();
			stack.clear();
			return node;
		}

		auto node = std::make_shared<Nodes::Identifier>(id);
		node->line = tokens[t].line;
		node->pos = tokens[t].pos;
		return node;
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
		stack.push_back(tokens[t - 1]);
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

bool Parser::ParenthesesR(bool error)
{
	if(!Symbol(ReservedSymbol::ParenthesesR))
	{
		if(error) throw ParseException("Expected a ')' ", tokens[t]);
		return false;
	}
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
	if(Symbol(ReservedSymbol::BracketL)) stack.push_back(tokens[t - 1]);
	else
	{
		if(error) throw ParseException("Expected a '[' ", tokens[t]);
		return false;
	}
	return true;
}

bool Parser::BracketR()
{
	if(Symbol(ReservedSymbol::BracketR)) stack.push_back(tokens[t - 1]); 
	else throw ParseException("Expected a ']' ", tokens[t]);
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
	if(Word(ReservedWord::If))
	{
		AddIf();
		return true;
	}
	return false;
}

bool Parser::Else()
{
	if(Word(ReservedWord::Else))
	{
		AddElse();
		return true;
	}
	return false;
}

bool Parser::While(bool error)
{
	if(!Word(ReservedWord::While))
	{
		if(error) throw ParseException("Expected the keyword 'while'", tokens[t]);
		return false;
	}
	if(!error) AddWhile();
	return true;
}

bool Parser::Do()
{
	if(Word(ReservedWord::Do))
	{
		AddDoWhile();
		return true;
	}
	return false;
}

bool Parser::For()
{
	if(Word(ReservedWord::For))
	{
		AddFor();
		return true;
	}
	return false;
}

bool Parser::Int()
{
	if(!Word(ReservedWord::Int)) throw ParseException("Expected type 'int'", tokens[t]);
	else stack.push_back(tokens[t - 1]);
	return true;
}