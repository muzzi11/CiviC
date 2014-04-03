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

void Parser::ParseProgram()
{
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
	return Semicolon();
}

bool Parser::GlobalDec()
{
	return Semicolon();
}

bool Parser::Def()
{
	if(Void() && Id(true) && FunHeader(true) && FunDef())
	{
		return true;
	}
	else if(Type())
	{
		return ArrayExpr() && Id(true) && AssignOpt() && GlobalDef() ||
			Id() && (FunHeader() && FunDef() || AssignOpt() && GlobalDef());
	}

	return false;
}

bool Parser::Export()
{
	Word(ReservedWord::Export);
	return true;
}

bool Parser::GlobalDef()
{
	return Semicolon();
}

bool Parser::FunDef()
{
	return BraceL(true) && FunBody() && BraceR();
}

bool Parser::Type(bool error)
{
	if(Word(ReservedWord::Bool))
	{
		return true;
	}
	else if(Word(ReservedWord::Int))
	{
		return true;
	}
	else if(Word(ReservedWord::Float))
	{
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
		if(ArrayExpr() && Id(true) && AssignOpt() && Semicolon() && Locals() ||
			Id() && (LocalFun() && LocalFuns() || AssignOpt() && Semicolon() && Locals()))
		{
			return true;
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
	return FunHeader(error) && BraceL(true) && FunBody() && BraceR();
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
	return Word(ReservedWord::Return) && Expr() && Semicolon() || true;
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

/*bool Parser::Expr(bool error)
{
	std::vector<Token> output;
	std::vector<Operator> stack, argStack;
	size_t end;

	int parentheses = 0, brackets = 0;
	for(end = t; end < tokens.size(); ++end)
	{
		const Token& token = tokens[end];

		if(token == ReservedSymbol::Semicolon) break;
		else if(token == ReservedSymbol::ParenthesesL) parentheses++;
		else if(token == ReservedSymbol::ParenthesesR) parentheses--;
		else if(token == ReservedSymbol::BracketL) brackets++;
		else if(token == ReservedSymbol::BracketR) brackets--;

		if(parentheses < 0 || brackets < 0) break;
	}

	for(; t < end; ++t)
	{
		const Token& token = tokens[t];

		if(token.type == TokenType::BoolType || token.type == TokenType::IntType || token.type == TokenType::FloatType)
		{
			output.push_back(token);
		}
		else if(token.type == TokenType::Identifier)
		{
			stack.emplace_back(t, token);
		}
		else if(token == ReservedSymbol::Comma)
		{
			while(!stack.empty() && stack.back().token != ReservedSymbol::ParenthesesL)
			{
				output.push_back(stack.back().token);
				stack.pop_back();
			}
		}
		else if(Presedence(t) > 0)
		{
			while(!stack.empty())
			{
				Operator op = stack.back();
				int presOp = Presedence(op.t);
				int presCur = Presedence(t);

				if(presOp > 0 && (!RightAssociative(t) && presCur == presOp || presCur < presOp))
				{
					output.push_back(op.token);
					stack.pop_back();
				}
				else break;
			}
			stack.emplace_back(t, token);
		}
		else if(token == ReservedSymbol::ParenthesesL)
		{
			// check if paren belongs to a function
			if(tokens[t - 1].type == TokenType::Identifier) argStack.emplace_back(t, token);
			stack.emplace_back(t, tokens[t]);
		}
		else if(token == ReservedSymbol::ParenthesesR)
		{
			for(;;)
			{
				if(stack.empty())
				{
					if(error) throw ParseException("Parentheses mismatch in expression", token);
					return false;
				}

				if(stack.back().token == ReservedSymbol::ParenthesesL)
				{
					stack.pop_back();
					break;
				}
				output.push_back(stack.back().token);
				stack.pop_back();
			}
		}
		else
		{
			if(error) throw ParseException("Unexpected token in expression", token);
			return false;
		}
	}

	while(!stack.empty())
	{
		const Token& token = stack.back().token;

		if(token == ReservedSymbol::ParenthesesL || token == ReservedSymbol::ParenthesesR)
		{
			if(error) throw ParseException("Parentheses mismatch in expression", token);
			return false;
		}
		else
		{
			output.push_back(token);
		}

		stack.pop_back();
	}

	return true;
}*/

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
		t++;
		return true;
	}

	if(error) throw ParseException("Missing identifier", tokens[t]);

	return false;
}

bool Parser::Void()
{
	return Word(ReservedWord::Void);
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