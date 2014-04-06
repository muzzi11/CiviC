#include <iostream>
#include <algorithm>

#include "analysis.h"
#include "symboltable.h"

Analyzer::Analyzer()
{
	sheaf.InitializeScope();
}

void Analyzer::Analyse(Nodes::NodePtr root, bool checkGlobalDef)
{
	if (!root->children.empty())
	{
		for (auto child : root->children)
		{
			ConsultTable(child, checkGlobalDef);
			TypeCheck(child);
		}
	}
}

void Analyzer::TypeCheck(Nodes::NodePtr node)
{
	if (node->Family() == Nodes::Assignment::Family())
	{
		TypeCheckAssigment(node);
	}
	else if(node->Family() == Nodes::UnaryOp::Family())
	{
		// unary type check
	}
	else if (node->Family() == Nodes::BinaryOp::Family())
	{
		TypeCheckBinOp(node);
	}
	else if (node->Family() == Nodes::Call::Family())
	{
		TypeCheckFuncArgs(node);
	}
	else if (node->Family() == Nodes::Return::Family())
	{
		TypeCheckFuncReturn(node);
	}
}

void Analyzer::TypeCheckAssigment(Nodes::NodePtr node)
{	
	auto ass = std::static_pointer_cast<Nodes::Assignment>(node);
	auto record = sheaf.LookUp(ass->name);
	if (record)
	{
		if (record->immutable && record->initialized)
		{
			std::cout << "The identifier " << ass->name << " is immutable at line " << node->line << " column " << node->pos << std::endl;

		}
		else
		{
			record->initialized = true;
			for (auto child : ass->children)
				TypeCheck(child, record->type);
		}		
	}		
}

void Analyzer::TypeCheckBinOp(Nodes::NodePtr node)
{
	auto binOp = std::static_pointer_cast<Nodes::BinaryOp>(node);
	Nodes::Type type = Nodes::Type::None;
	
	auto left = binOp->children[0];
	auto right = binOp->children[1];

	auto leftType = GetType(left);
	auto rightType = GetType(right);
	if (leftType != rightType) std::cout << "Operator types differ at line " << left->line << " column " << left->pos << std::endl;
	else binOp->type = leftType;
}

void Analyzer::TypeCheckFuncArgs(Nodes::NodePtr node)
{
	auto funcCall = std::static_pointer_cast<Nodes::Call>(node);
	auto record = sheaf.LookUp(funcCall->name);
		
	if (record)
	{
		if (record->funcArgs.size() != funcCall->children.size())
		{
			std::cout << "Arg count does not match function definition at line " << node->line << " column " << node->pos << std::endl;
			return;
		}
		for (size_t i = 0; i < funcCall->children.size(); ++i)
		{
			TypeCheck(funcCall->children[i], record->funcArgs[i]);
		}
	}	
}

void Analyzer::TypeCheckFuncReturn(Nodes::NodePtr node)
{
	auto returnVal = std::static_pointer_cast<Nodes::Return>(node);
	auto record = sheaf.LookUp(returnVal->functionName);
	if (record)
		for (auto child : returnVal->children)
			TypeCheck(child, record->type);
}

Nodes::Type Analyzer::GetType(Nodes::NodePtr node)
{	
	if (node->Family() == Nodes::Literal::Family())
	{
		auto lit = std::static_pointer_cast<Nodes::Literal>(node);
		return lit->type;
	}
	else if (node->Family() == Nodes::Identifier::Family())
	{
		auto id = std::static_pointer_cast<Nodes::Identifier>(node);
		auto record = sheaf.LookUp(id->name);
		if (record) return record->type;
	}
	else if (node->Family() == Nodes::Cast::Family())
	{
		auto cast = std::static_pointer_cast<Nodes::Cast>(node);
		return cast->type;
	}
	else if (node->Family() == Nodes::Call::Family())
	{
		auto call = std::static_pointer_cast<Nodes::Call>(node);
		auto record = sheaf.LookUp(call->name);
		if (record) return record->type;
	}

	for(auto child : node->children)
	{
		auto type = GetType(child);
		if(type != Nodes::Type::None) return type;
	}

	return Nodes::Type::None;
}

void Analyzer::TypeCheck(Nodes::NodePtr node, Nodes::Type type)
{
	if (node->Family() == Nodes::BinaryOp::Family())
	{
		auto binOp = std::static_pointer_cast<Nodes::BinaryOp>(node);
		for (auto child : binOp->children)
			TypeCheck(child, type);
	}
	else if (node->Family() == Nodes::Literal::Family())
	{
		auto lit = std::static_pointer_cast<Nodes::Literal>(node);
		if (lit->type != type)
			std::cout << "Literal is not of type " << Nodes::TypeToString(type) << " at line " << lit->line << " column " << lit->pos << std::endl;
	}
	else if (node->Family() == Nodes::Identifier::Family())
	{
		auto id = std::static_pointer_cast<Nodes::Identifier>(node);
		auto record = sheaf.LookUp(id->name);
		if (record && record->type != type)
			std::cout << "Identifier " << id->name << " is not of type " << Nodes::TypeToString(type) << " at line " << node->line << " column " << node->pos << std::endl;
	}
	else if (node->Family() == Nodes::Cast::Family())
	{
		auto cast = std::static_pointer_cast<Nodes::Cast>(node);
		if (cast->type != type)
			std::cout << "Cast is not of type " << Nodes::TypeToString(type) << std::endl;
	}
	else if (node->Family() == Nodes::Call::Family())
	{
		auto call = std::static_pointer_cast<Nodes::Call>(node);
		auto record = sheaf.LookUp(call->name);
		if (record && record->type != type)
			std::cout << "Function call " << call->name << " does not return of type " << Nodes::TypeToString(type) << " at line " << node->line << " column " << node->pos << std::endl;
	}
	else if (node->Family() == Nodes::ArrayExpr::Family())
	{
		auto arrayExpr = std::static_pointer_cast<Nodes::Call>(node);
	
	}
}

void Analyzer::ConsultTable(Nodes::NodePtr node, bool checkGlobalDef)
{	
	if(node->IsFamily<Nodes::GlobalDef>())
	{
		InsertGlobalDef(node);
	}
	else if(node->IsFamily<Nodes::GlobalDec>())
	{
		InsertGlobalDec(node);
	}
	else if(node->IsFamily<Nodes::FunctionDef>())
	{
		InsertFuncDef(node);
	}
	else if(node->IsFamily<Nodes::FunctionDec>())
	{
		InsertFuncDec(node);
	}
	else if(node->IsFamily<Nodes::VarDec>())
	{
		InsertVarDec(node);
	}
	else if(node->IsFamily<Nodes::Call>())
	{
		LookUpCall(node);
	}
	else if(node->IsFamily<Nodes::Identifier>())
	{
		LookUpIdentifier(node);
		if (checkGlobalDef) CheckGlobalDef(node);
	}
	else if(node->IsFamily<Nodes::Assignment>())
	{
		LookUpAssignment(node);
	}

	// Function def traversal is done in the InsertFunDef function
	if(!node->IsFamily<Nodes::FunctionDef>())
		Analyse(node, checkGlobalDef);
}

void Analyzer::InsertGlobalDef(Nodes::NodePtr node)
{
	auto globDef = std::static_pointer_cast<Nodes::GlobalDef>(node);
	auto record = SymbolTable::Record(false, globDef->var.type, node);
	/*
	if (globDef->var.array)
	{
		auto arrayExpr = globDef->children[0];
		for (auto )
	}*/

	sheaf.Insert(globDef->var.name, record);
	globalDefs.push_back(globDef->var.name);
}

void Analyzer::InsertGlobalDec(Nodes::NodePtr node)
{
	auto globDec = std::static_pointer_cast<Nodes::GlobalDec>(node);
	auto record = SymbolTable::Record(false, globDec->param.type, node);
	sheaf.Insert(globDec->param.name, record);
}

void Analyzer::InsertFuncDef(Nodes::NodePtr node)
{
	auto funDef = std::static_pointer_cast<Nodes::FunctionDef>(node);

	if (funDef->header.name.compare("__init") == 0)
	{
		ProcesInitFunc(funDef);
		return;
	}

	auto funcRecord = SymbolTable::Record(false, funDef->header.returnType, node);		
	for (auto param : funDef->header.params)	
		funcRecord.funcArgs.push_back(param.type);
	sheaf.Insert(funDef->header.name, funcRecord);

	//Create new scope
	sheaf.InitializeScope();
	//Add every parameter to the newly created scope
	for (auto param : funDef->header.params)
	{
		auto record = SymbolTable::Record(false, param.type, node);
		sheaf.Insert(param.name, record);
	}
		
	//Add the declarations/definitions inside the function body to the scope
	for (auto child : funDef->children)
	{
		ConsultTable(child);
		TypeCheck(child);
	}
	sheaf.FinalizeScope();
}

void Analyzer::InsertFuncDec(Nodes::NodePtr node)
{
	auto funDec = std::static_pointer_cast<Nodes::FunctionDec>(node);
	auto record = SymbolTable::Record(false, funDec->header.returnType, node);	
	sheaf.Insert(funDec->header.name, record);
}

void Analyzer::InsertVarDec(Nodes::NodePtr node)
{
	auto varDec = std::static_pointer_cast<Nodes::VarDec>(node);
	auto record = SymbolTable::Record(varDec->immutable, varDec->var.type, node);
	sheaf.Insert(varDec->var.name, record);
}

void Analyzer::LookUpCall(Nodes::NodePtr node)
{
	auto funCall = std::static_pointer_cast<Nodes::Call>(node);
	auto record = sheaf.LookUp(funCall->name);
	if (!record)
		std::cout << "Unkown function " << funCall->name << " at line " << node->line << " column " << node->pos << std::endl;
	else
		funCall->decPtr = record->decPtr;
}

void Analyzer::LookUpIdentifier(Nodes::NodePtr node)
{	
	auto identifier = std::static_pointer_cast<Nodes::Identifier>(node);
	auto record = sheaf.LookUp(identifier->name);
	if (!record)
		std::cout << "Unkown identifier " << identifier->name << " at line " << node->line << " column " << node->pos << std::endl;
	else
		identifier->defOrDec = record->decPtr;
}

void Analyzer::LookUpAssignment(Nodes::NodePtr node)
{
	auto assignment = std::static_pointer_cast<Nodes::Assignment>(node);
	auto record = sheaf.LookUp(assignment->name);
	if(record)
	{
		assignment->dec = record->decPtr;
		assignment->type = record->type;
	}
	else std::cout << "Unknown identifier " << assignment->name << " at line " << node->line << " column " << node->pos << std::endl;
}

void Analyzer::ProcesInitFunc(std::shared_ptr<Nodes::FunctionDef> funcDef)
{
	auto funcRecord = SymbolTable::Record(false, funcDef->header.returnType, funcDef);
	
	for (auto child : funcDef->children)
	{		
		ConsultTable(child, true);
		TypeCheck(child);
	}
	sheaf.FinalizeScope();
}

void Analyzer::CheckGlobalDef(Nodes::NodePtr node)
{	
	auto identifier = std::static_pointer_cast<Nodes::Identifier>(node);
	if (std::find(globalDefs.begin(), globalDefs.end(), identifier->name) != globalDefs.end())
		std::cout << "Unkown identifier " << identifier->name << " at line " << node->line << " column " << node->pos << std::endl;
}