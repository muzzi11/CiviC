#include <iostream>
#include <algorithm>

#include "analysis.h"
#include "symboltable.h"

Analyzer::Analyzer()
{
	sheaf.InitializeScope();
}

std::string Analyzer::Analyse(Nodes::NodePtr root, bool checkGlobalDef)
{
	if (!root->children.empty())
	{
		for (auto child : root->children)
		{
			ConsultTable(child, checkGlobalDef);
			TypeCheck(child);
		}
	}
	return errors.str();
}

void Analyzer::TypeCheck(Nodes::NodePtr node)
{
	if (node->IsFamily<Nodes::Assignment>())
	{
		TypeCheckAssigment(node);
	}
	else if (node->IsFamily<Nodes::UnaryOp>())
	{
		TypeCheckUnary(node);
	}
	else if (node->IsFamily<Nodes::BinaryOp>())
	{
		TypeCheckBinOp(node);
	}
	else if (node->IsFamily<Nodes::Call>())
	{
		TypeCheckFuncArgs(node);
	}
	else if (node->IsFamily<Nodes::Return>())
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
			PrintErrorInfo(node->pos, node->line);
			errors << "The identifier " << ass->name << " is immutable" << std::endl;
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
	if (leftType != rightType)
	{
		PrintErrorInfo(left->pos, left->line);
		errors << "The operator's operands are of different types" << std::endl;
	}
	else if (binOp->op == Nodes::Operator::Modulo && (leftType != Nodes::Type::Int && leftType != Nodes::Type::Int))
	{
		PrintErrorInfo(left->pos, left->line);
		errors << "Both operands of the modulo operator need to be of type int." << std::endl;
	}
	else
	{
		binOp->type = leftType;
	}
}

void Analyzer::TypeCheckUnary(Nodes::NodePtr node)
{
	auto unOp = std::static_pointer_cast<Nodes::UnaryOp>(node);
	unOp->type = GetType(unOp);
}

void Analyzer::TypeCheckFuncArgs(Nodes::NodePtr node)
{
	auto funcCall = std::static_pointer_cast<Nodes::Call>(node);
	auto record = sheaf.LookUp(funcCall->name);
		
	if (record)
	{
		if (record->funcArgs.size() != funcCall->children.size())
		{
			PrintErrorInfo(node->pos, node->line);
			errors << "Arg count does not match function definition" << std::endl;
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
	if(record)
	{
		for(auto child : returnVal->children)
			TypeCheck(child, record->type);

		returnVal->type = GetType(returnVal);
	}
	returnVal->type = GetType(returnVal);
}

Nodes::Type Analyzer::GetType(Nodes::NodePtr node)
{	
	if (node->IsFamily<Nodes::Literal>())
	{
		auto lit = std::static_pointer_cast<Nodes::Literal>(node);
		return lit->type;
	}
	else if (node->IsFamily<Nodes::Identifier>())
	{
		auto id = std::static_pointer_cast<Nodes::Identifier>(node);
		auto record = sheaf.LookUp(id->name);
		if (record) return record->type;
	}
	else if(node->IsFamily<Nodes::Cast>())
	{
		auto cast = std::static_pointer_cast<Nodes::Cast>(node);
		return cast->type;
	}
	else if(node->IsFamily<Nodes::Call>())
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
	if (node->IsFamily<Nodes::BinaryOp>())
	{
		auto binOp = std::static_pointer_cast<Nodes::BinaryOp>(node);
		for (auto child : binOp->children)
			TypeCheck(child, type);
	}
	else if(node->IsFamily<Nodes::Literal>())
	{
		auto lit = std::static_pointer_cast<Nodes::Literal>(node);
		if (lit->type != type)
		{
			PrintErrorInfo(node->pos, node->line);
			errors << "Literal is not of type " << Nodes::TypeToString(type) << std::endl;
		}
	}
	else if(node->IsFamily<Nodes::Identifier>())
	{
		auto id = std::static_pointer_cast<Nodes::Identifier>(node);
		auto record = sheaf.LookUp(id->name);
		if (record && record->type != type)
		{
			PrintErrorInfo(node->pos, node->line);
			errors << "Identifier " << id->name << " is not of type " << Nodes::TypeToString(type) << std::endl;
		}
	}
	else if(node->IsFamily<Nodes::Cast>())
	{
		auto cast = std::static_pointer_cast<Nodes::Cast>(node);
		if (cast->type != type)
		{
			PrintErrorInfo(node->pos, node->line);
			errors << "Cast is not of type " << Nodes::TypeToString(type) << std::endl;
		}
	}
	else if(node->IsFamily<Nodes::Call>())
	{
		auto call = std::static_pointer_cast<Nodes::Call>(node);
		auto record = sheaf.LookUp(call->name);
		if (record && record->type != type)
		{
			PrintErrorInfo(node->pos, node->line);
			errors << "Function call " << call->name << " does not return of type " << Nodes::TypeToString(type) << std::endl;
		}
	}
	else if(node->IsFamily<Nodes::ArrayExpr>())
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
	{
		PrintErrorInfo(node->pos, node->line);
		errors << "Unkown function " << funCall->name << std::endl;
	}
	else
	{
		funCall->dec = record->decPtr;
	}
}

void Analyzer::LookUpIdentifier(Nodes::NodePtr node)
{	
	auto identifier = std::static_pointer_cast<Nodes::Identifier>(node);
	auto record = sheaf.LookUp(identifier->name);
	if (!record)
	{
		PrintErrorInfo(node->pos, node->line);
		errors << "Unkown identifier " << identifier->name << std::endl;
	}
	else
	{
		identifier->dec = record->decPtr;
		identifier->type = record->type;
	}
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
	else
	{
		PrintErrorInfo(node->pos, node->line);
		errors << "Unknown identifier " << assignment->name << std::endl;
	}
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
	{
		PrintErrorInfo(node->pos, node->line);
		errors << "Unkown identifier " << identifier->name << std::endl;
	}
}

void Analyzer::PrintErrorInfo(const int pos, const int line)
{
	errors << "Error at line " << line << " column " << pos << ": ";
}