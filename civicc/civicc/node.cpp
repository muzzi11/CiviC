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

std::string GlobalDef::ToString() const
{
	return exp ? "export " + var.ToString() : var.ToString();
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