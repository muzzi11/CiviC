#include <sstream>

#include "parser.h"


ParseException::ParseException(const std::string& str, const Token& token)
{
	std::stringstream sstream(str);
	sstream << str << " at line " << token.line << " column " << token.pos << "\n";
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
	if(Void() && Id(true) && LocalFun(true) && Locals())
	{
		return true;
	}
	else if(Type())
	{
		if(ArrayExpr() && Id(true) && AssignOpt() && Semicolon() && Locals() ||
			Id() && (LocalFun() && Locals() || AssignOpt() && Semicolon() && Locals()))
		{
			return true;
		}
		else
		{
			throw ParseException("Expected a function definition or ';'", tokens[t]);
		}
	}
	
	return true;
}

bool Parser::LocalFun(bool error)
{
	return FunHeader(error) && BraceL(true) && FunBody() && BraceR();
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
	/*if(Id())
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
	}*/
	return Id() && (ArrayExpr() && Assign() && Semicolon() ||
		Assign() && Semicolon() ||
		ParenthesesL() && (ParenthesesR() && Semicolon() || Expr() && Exprs() && ParenthesesR() && Semicolon())) ||
		If() && ParenthesesL() && Expr() && ParenthesesR() && Block() && ElseBlock() ||
		While() && ParenthesesL() && Expr() && ParenthesesR() && Block() ||
		Do() && Block() && While() && ParenthesesL() && Expr() && ParenthesesR() && Semicolon() ||
		For() && ParenthesesL() && Int() && Id(true) && Assign() && Comma() && Expr() && Step() && ParenthesesR() && Block();
}

bool Parser::Statements()
{
	return Statement() && Statements() || true;
}

bool Parser::Step()
{
	size_t tOld = t;
	return Comma() && Expr() || (tOld == t);
}

bool Parser::Block()
{
	return BraceL(true) && Statements() && BraceR() || Statement();
}

bool Parser::ElseBlock()
{
	return Else() && Block() || true;
}

bool Parser::Return()
{
	size_t tOld = t;
	return Word(ReservedWord::Return) && Expr() && Semicolon() || (tOld == t);
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
	std::vector<Token> output;
	std::vector<size_t> stack;
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
			stack.push_back(t);
		}
		else if(token == ReservedSymbol::Comma)
		{
			while(!stack.empty() && tokens[stack.back()] != ReservedSymbol::ParenthesesL)
			{
				output.push_back(tokens[stack.back()]);
				stack.pop_back();
			}
		}
		else if(Presedence(t) > 0)
		{
			while(!stack.empty() && Presedence(stack.back()) > 0 && (!RightAssociative(t) && Presedence(t) == Presedence(stack.back()) || Presedence(t) < Presedence(stack.back())))
			{
				output.push_back(tokens[stack.back()]);
				stack.pop_back();
			}
			stack.push_back(t);
		}
		else if(token == ReservedSymbol::ParenthesesL)
		{
			stack.push_back(t);
		}
		else if(token == ReservedSymbol::ParenthesesR)
		{
			for(;;)
			{
				if(stack.empty())
				{
					// parentheses mismatch
					return false;
				}

				if(tokens[stack.back()] == ReservedSymbol::ParenthesesL)
				{
					stack.pop_back();
					break;
				}
				output.push_back(tokens[stack.back()]);
				stack.pop_back();
			}
		}
		else
		{
			// unexpected token
			return false;
		}
	}

	while(!stack.empty())
	{
		const Token& token = tokens[stack.back()];

		if(token == ReservedSymbol::ParenthesesL || token == ReservedSymbol::ParenthesesR)
		{
			// parentheses mismatch
			return false;
		}
		else
		{
			output.push_back(token);
		}

		stack.pop_back();
	}

	return true;
}

bool Parser::Exprs()
{
	size_t tOld = t;
	return Comma() && Expr() && Exprs() || (tOld == t);
}

unsigned int Parser::Presedence(size_t tokenIndex)
{
	if(tokenIndex == 0) return 0;

	const Token& token = tokens[tokenIndex];
	const unsigned int addPresedence = 300, minusPresedence = addPresedence;

	if(token == ReservedSymbol::And || token == ReservedSymbol::Or) return 100;
	else if(token == ReservedSymbol::Equals || token == ReservedSymbol::Unequals || token == ReservedSymbol::Less ||
		token == ReservedSymbol::LessEqual || token == ReservedSymbol::More || token == ReservedSymbol::MoreEqual) return 200;
	else if(token == ReservedSymbol::Plus) return addPresedence;
	else if(token == ReservedSymbol::Multiply || token == ReservedSymbol::Divide || token == ReservedSymbol::Modulo) return 400;
	else if(token == ReservedSymbol::Not) return unaryPresedence;
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
			return unaryPresedence;
		}
	}

	return 0;
}

bool Parser::RightAssociative(size_t tokenIndex)
{
	const Token& token = tokens[tokenIndex];

	if(token == ReservedSymbol::Not) return true;
	else if(token == ReservedSymbol::Minus) return Presedence(tokenIndex) == unaryPresedence;

	return false;
}


bool Parser::Word(ReservedWord word)
{
	if(tokens[t] == word)
	{
		t++;
		return true;
	}

	return false;
}

bool Parser::Symbol(ReservedSymbol symbol)
{
	if(tokens[t] == symbol)
	{
		t++;
		return true;
	}

	return false;
}

bool Parser::Id(bool error)
{
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
	if(!Symbol(ReservedSymbol::BraceR)) throw ParseException("Expected a '}' ", tokens[t]);
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

bool Parser::Comma()
{
	return Symbol(ReservedSymbol::Comma);
}

bool Parser::Semicolon()
{
	if(!Symbol(ReservedSymbol::Semicolon)) throw ParseException("Expected a ';' ", tokens[t]);
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

bool Parser::While()
{
	return Word(ReservedWord::While);
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
	return Word(ReservedWord::Int);
}