#include <sstream>

#include "node.h"

using namespace Node;


int BaseNode::familyCounter_ = 0;
std::unordered_map<int, std::string> BaseNode::familyNames_(1);

std::string BaseNode::ToString() const
{
	return "";
}

int BaseNode::Family() const
{
	return family_;
}

std::string BaseNode::FamilyName() const
{
	return familyNames_[family_];
}


std::string TypeToString(Type type)
{
	static const std::unordered_map<Type, std::string> map(
	{
		{ Type::Bool, "bool" },
		{ Type::Int, "int" },
		{ Type::Float, "float"},
		{ Type::Void, "void" },
	});

	return map.at(type);
}

std::string OperatorToString(Operator op)
{
	static const std::unordered_map<Operator, std::string> map(
	{
		{ Operator::Add, "+" },
		{ Operator::Subtract, "-" },
		{ Operator::Multiply, "*" },
		{ Operator::Divide, "/" },
		{ Operator::Modulo, "%" },
		{ Operator::Equal, "==" },
		{ Operator::NotEqual, "!=" },
		{ Operator::Less, "<" },
		{ Operator::LessEqual, "<=" },
		{ Operator::More, ">" },
		{ Operator::MoreEqual, ">=" },
		{ Operator::And, "&&" },
		{ Operator::Or, "||" },
		{ Operator::Negate, "-" },
		{ Operator::Not, "!" },
	});

	return map.at(op);
}

std::string Variable::ToString() const
{
	return TypeToString(type) + ' ' + name;
}

std::string Param::ToString() const
{
	std::string str = TypeToString(type) + ' ' + name;
	if(!dim.empty())
	{
		str += '[';
		for(const auto& d : dim) str += d + ',';
		str.pop_back();
	}
	return str + ']';
}

std::string FunctionHeader::ToString() const
{
	std::string str = TypeToString(returnType) + ' ' + name + '(';
	for(const auto& p : params) str += p.ToString() + ',';
	if(!params.empty()) str.pop_back();
	return str + ')';
}

std::string FunctionDec::ToString() const
{
	return header.ToString();
}

std::string GlobalDec::ToString() const
{
	return "extern " + param.ToString();
}

std::string FunctionDef::ToString() const
{
	return exp ? "export " + header.ToString() : header.ToString();
}

bool GlobalDef::HasAssignment() const
{
	return var.array ? children.size() > 1 : !children.empty();
}

std::string GlobalDef::ToString() const
{
	return exp ? "export " + var.ToString() : var.ToString();
}

bool VarDec::HasAssignment() const
{
	return var.array ? children.size() > 1 : !children.empty();
}

std::string VarDec::ToString() const
{
	return var.ToString();
}

std::string Assignment::ToString() const
{
	return name;
}

std::string Call::ToString() const
{
	return name;
}

std::string BinaryOp::ToString() const
{
	if(children.size() != 2) return "";
	else return "(" + children[0]->ToString() + " " + OperatorToString(op) + " " + children[1]->ToString() + ")";
}

std::string UnaryOp::ToString() const
{
	if(children.size() != 1) return "";
	else return OperatorToString(op) + children[0]->ToString();
}

std::string Cast::ToString() const
{
	return TypeToString(type);
}

std::string Literal::ToString() const
{
	std::stringstream sstream;

	if(type == Type::Bool) sstream << boolValue;
	else if(type == Type::Int) sstream << intValue;
	else sstream << floatValue;

	return sstream.str();
}

std::string Identifier::ToString() const
{
	return name + (children.empty() ? "" : "[]");
}

std::string Ternary::ToString() const
{
	if(children.size() != 3) return "";
	else return children[0]->ToString() + " ? " + children[1]->ToString() + " : " + children[2]->ToString();
}