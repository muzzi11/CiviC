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
		auto ass = std::static_pointer_cast<Node::Assignment>(node);

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
	auto record = SymbolTable::Record(false, funDef->header.returnType);
	sheaf.Insert(funDef->header.name, record);

	//Create new scope
	sheaf.InitializeScope();
	//Add every parameter to the newly created scope
	for (auto param : funDef->header.params)
	{
		auto record = SymbolTable::Record(false, param.type);
		sheaf.Insert(param.name, record);
	}

	//Add the declarations/definitions inside the function body to the scope
	for (auto funnode : funDef->children)
		ConsultTable(funnode);	
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