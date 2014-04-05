#include <iostream>

#include "analysis.h"
#include "symboltable.h"

Analyzer::Analyzer()
{
	sheaf.InitializeScope();
}

void Analyzer::Analyse(Node::NodePtr root)
{
	if (!root->children.empty())
	{
		for (auto child : root->children)
		{
			ConsultTable(child);
			TypeCheck(child);
		}
	}
}

void Analyzer::TypeCheck(Node::NodePtr node)
{
	if (node->Family() == Node::Assignment::Family())
	{
		TypeCheckAssigment(node);
	}
	else if (node->Family() == Node::BinaryOp::Family())
	{
		TypeCheckBinOp(node);
	}
	else if (node->Family() == Node::Call::Family())
	{
		TypeCheckFuncArgs(node);
	}
	else if (node->Family() == Node::Return::Family())
	{
		
	}
}

void Analyzer::TypeCheckAssigment(Node::NodePtr node)
{	
	auto ass = std::static_pointer_cast<Node::Assignment>(node);
	auto record = sheaf.LookUp(ass->name);
	if (record)
		for (auto child : ass->children)
			TypeCheck(child, record->type);	
}

void Analyzer::TypeCheckBinOp(Node::NodePtr node)
{
	auto binOp = std::static_pointer_cast<Node::BinaryOp>(node);
	Node::Type type = Node::Type::None;
	
	auto left = binOp->children[0];
	auto right = binOp->children[1];

	auto leftType = GetType(left);
	auto rightType = GetType(right);
	if (leftType != rightType) std::cout << "Operator types differ" << std::endl;
}

void Analyzer::TypeCheckFuncArgs(Node::NodePtr node)
{
	auto funcCall = std::static_pointer_cast<Node::Call>(node);
	auto record = sheaf.LookUp(funcCall->name);
		
	if (record)
	{
		if (record->funcArgs.size() != funcCall->children.size())
			std::cout << "Arg count does not match function definition." << std::endl;
		for (int i = 0; i < funcCall->children.size(); ++i)
		{
			TypeCheck(funcCall->children[i], record->funcArgs[i]);
		}
	}	
}

void Analyzer::TypeCheckFuncReturn(Node::NodePtr node)
{
	auto returnVal = std::static_pointer_cast<Node::Return>(node);
	//for (auto child : returnVal->children)
		//child->
}

Node::Type Analyzer::GetType(Node::NodePtr node)
{	
	if (node->Family() == Node::Literal::Family())
	{
		auto lit = std::static_pointer_cast<Node::Literal>(node);
		return lit->type;
	}
	else if (node->Family() == Node::Identifier::Family())
	{
		auto id = std::static_pointer_cast<Node::Identifier>(node);
		auto record = sheaf.LookUp(id->name);
		if (record) return record->type;
	}
	else if (node->Family() == Node::Cast::Family())
	{
		auto cast = std::static_pointer_cast<Node::Cast>(node);
		return cast->type;
	}
	else if (node->Family() == Node::Call::Family())
	{
		auto call = std::static_pointer_cast<Node::Call>(node);
		auto record = sheaf.LookUp(call->name);
		if (record) return record->type;
	}
}

void Analyzer::TypeCheck(Node::NodePtr node, Node::Type type)
{
	if (node->Family() == Node::BinaryOp::Family())
	{
		auto binOp = std::static_pointer_cast<Node::BinaryOp>(node);
		for (auto child : binOp->children)
			TypeCheck(child, type);
	}
	else if (node->Family() == Node::Literal::Family())
	{
		auto lit = std::static_pointer_cast<Node::Literal>(node);
		if (lit->type != type)
			std::cout << "Literal is not of type " << (int)type << std::endl;
	}
	else if (node->Family() == Node::Identifier::Family())
	{
		auto id = std::static_pointer_cast<Node::Identifier>(node);
		auto record = sheaf.LookUp(id->name);
		if (record && record->type != type)
			std::cout << "Identifier " << id->name << " is not of type " << (int)type << std::endl;
	}
	else if (node->Family() == Node::Cast::Family())
	{
		auto cast = std::static_pointer_cast<Node::Cast>(node);
		if (cast->type != type)
			std::cout << "Cast is not of type " << (int)type << std::endl;
	}
	else if (node->Family() == Node::Call::Family())
	{
		auto call = std::static_pointer_cast<Node::Call>(node);
		auto record = sheaf.LookUp(call->name);
		if (record && record->type != type)
			std::cout << "Function call " << call->name << " does not return of type " << (int)type << std::endl;
	}
	else if (node->Family() == Node::ArrayExpr::Family())
	{
		auto arrayExpr = std::static_pointer_cast<Node::Call>(node);
	
	}
}

void Analyzer::ConsultTable(Node::NodePtr node)
{	
	if (node->Family() == Node::GlobalDef::Family())
	{
		InsertGlobalDef(node);
	}
	else if (node->Family() == Node::GlobalDec::Family())
	{
		InsertGlobalDec(node);
	}
	else if (node->Family() == Node::FunctionDef::Family())
	{
		InsertFuncDef(node);
	}
	else if (node->Family() == Node::FunctionDec::Family())
	{
		InsertFuncDec(node);
	}
	else if (node->Family() == Node::VarDec::Family())
	{
		InsertVarDec(node);
	}
	else if (node->Family() == Node::Call::Family())
	{
		LookUpCall(node);
	}
	else if (node->Family() == Node::Identifier::Family())
	{
		LookUpIdentifier(node);
	}

	// Function def traversal is done in the InsertFunDef function
	if (node->Family() != Node::FunctionDef::Family())
		Analyse(node);
}

void Analyzer::InsertGlobalDef(Node::NodePtr node)
{
	auto globDef = std::static_pointer_cast<Node::GlobalDef>(node);
	auto record = SymbolTable::Record(false, globDef->var.type);

	/*
	if (globDef->var.array)
	{
		record.
	}*/

	sheaf.Insert(globDef->var.name, record);
}

void Analyzer::InsertGlobalDec(Node::NodePtr node)
{
	auto globDec = std::static_pointer_cast<Node::GlobalDec>(node);
	auto record = SymbolTable::Record(false, globDec->param.type);
	sheaf.Insert(globDec->param.name, record);
}

void Analyzer::InsertFuncDef(Node::NodePtr node)
{
	auto funDef = std::static_pointer_cast<Node::FunctionDef>(node);
	auto funcRecord = SymbolTable::Record(false, funDef->header.returnType);

	//Create new scope
	sheaf.InitializeScope();
	//Add every parameter to the newly created scope
	for (auto param : funDef->header.params)
	{
		auto record = SymbolTable::Record(false, param.type);
		sheaf.Insert(param.name, record);
		funcRecord.funcArgs.push_back(param.type);
	}
	sheaf.Insert(funDef->header.name, funcRecord);

	//Add the declarations/definitions inside the function body to the scope
	for (auto child : funDef->children)
	{
		ConsultTable(child);
		TypeCheck(child);
	}
	sheaf.FinalizeScope();
}

void Analyzer::InsertFuncDec(Node::NodePtr node)
{
	auto funDec = std::static_pointer_cast<Node::FunctionDec>(node);
	auto record = SymbolTable::Record(false, funDec->header.returnType);	
	sheaf.Insert(funDec->header.name, record);
}

void Analyzer::InsertVarDec(Node::NodePtr node)
{
	auto varDec = std::static_pointer_cast<Node::VarDec>(node);
	auto record = SymbolTable::Record(false, varDec->var.type);
	sheaf.Insert(varDec->var.name, record);
}

void Analyzer::LookUpCall(Node::NodePtr node)
{
	auto funCall = std::static_pointer_cast<Node::Call>(node);
	if (!sheaf.LookUp(funCall->name))
		std::cout << "Unkown function " << funCall->name << std::endl;
}

void Analyzer::LookUpIdentifier(Node::NodePtr node)
{
	auto identifier = std::static_pointer_cast<Node::Identifier>(node);
	if (!sheaf.LookUp(identifier->name))
		std::cout << "Unkown identifier " << identifier->name << std::endl;
}