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
	TypeCheckAssigment(node);
	TypeCheckUnary(node);
	TypeCheckBinOp(node);
	TypeCheckFuncArgs(node);
	TypeCheckFuncReturn(node);	
}

void Analyzer::TypeCheckAssigment(Nodes::NodePtr node)
{	
	auto ass = Nodes::StaticCast<Nodes::Assignment>(node);
	if (!ass) return;

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
			{
				if (isBoolOp(child)) return;
				TypeCheck(child, record->type);
			}
		}		
	}		
}

void Analyzer::TypeCheckBinOp(Nodes::NodePtr node)
{
	auto binOp = Nodes::StaticCast<Nodes::BinaryOp>(node);
	if (!binOp) return;

	Nodes::Type type = Nodes::Type::None;	
	auto left = binOp->children[0];
	auto right = binOp->children[1];
	auto leftType = GetType(left);
	auto rightType = GetType(right);

	if (binOp->op == Nodes::Operator::Modulo && (!IsNumber(leftType) || !IsNumber(rightType)))
	{
		PrintErrorInfo(left->pos, left->line);
		errors << "Both operands of the modulo operator must be of type int" << std::endl;
	}
	else if (binOp->op == Nodes::Operator::Divide && (!IsNumber(leftType) || !IsNumber(rightType)))
	{
		PrintErrorInfo(left->pos, left->line);
		errors << "Both operands of the division operator must be of type int or float" << std::endl;
	}
	else if (leftType != rightType)
	{
		PrintErrorInfo(left->pos, left->line);
		errors << "The operator's operands are of different types" << std::endl;
	}
	else
	{
		binOp->type = leftType;
	}
}

void Analyzer::TypeCheckUnary(Nodes::NodePtr node)
{
	auto unOp = Nodes::StaticCast<Nodes::UnaryOp>(node);
	if (!unOp) return;

	auto operandType = GetType(unOp);
	if (unOp->op == Nodes::Operator::Not && operandType != Nodes::Type::Bool)
	{
		PrintErrorInfo(node->pos, node->line);
		errors << "The operand of the negation operator must be of type bool" << std::endl;
	}
	else if (unOp->op == Nodes::Operator::Negate && operandType != Nodes::Type::Int && operandType != Nodes::Type::Float)
	{
		PrintErrorInfo(node->pos, node->line);
		errors << "The operand of the negation operator must be of type int or float" << std::endl;
	}

	unOp->type = operandType;
}

void Analyzer::TypeCheckFuncArgs(Nodes::NodePtr node)
{
	auto funcCall = Nodes::StaticCast<Nodes::Call>(node);
	if (!funcCall) return;

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
	auto returnVal = Nodes::StaticCast<Nodes::Return>(node);
	if (!returnVal) return;

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
	auto lit = Nodes::StaticCast<Nodes::Literal>(node);
	if (lit) return lit->type;	
	
	auto cast = Nodes::StaticCast<Nodes::Cast>(node);
	if (cast) return cast->type;

	auto id = Nodes::StaticCast<Nodes::Identifier>(node);
	if (id)
	{
		auto record = sheaf.LookUp(id->name);
		if (record) return record->type;
	}

	auto call = Nodes::StaticCast<Nodes::Call>(node);
	if (call)
	{
		auto record = sheaf.LookUp(call->name);
		if (record) return record->type;
	}			

	for (auto child : node->children)
	{
		auto type = GetType(child);
		if(type != Nodes::Type::None) return type;
	}
	return Nodes::Type::None;
}

void Analyzer::TypeCheck(Nodes::NodePtr node, Nodes::Type type)
{	
	auto binOp = Nodes::StaticCast<Nodes::BinaryOp>(node);
	if (binOp)
	{
		for (auto child : binOp->children)
			TypeCheck(child, type);
	}	
		
	auto lit = Nodes::StaticCast<Nodes::Literal>(node);
	if (lit && lit->type != type)
	{
		PrintErrorInfo(node->pos, node->line);
		errors << "Literal is not of type " << Nodes::TypeToString(type) << std::endl;
	}
		
	auto id = Nodes::StaticCast<Nodes::Identifier>(node);
	if (id)
	{
		auto record = sheaf.LookUp(id->name);
		if (record && record->type != type)
		{
			PrintErrorInfo(node->pos, node->line);
			errors << "Identifier " << id->name << " is not of type " << Nodes::TypeToString(type) << std::endl;
		}
	}
	
	auto cast = Nodes::StaticCast<Nodes::Cast>(node);
	if (cast && cast->type != type)
	{
		PrintErrorInfo(node->pos, node->line);
		errors << "Cast is not of type " << Nodes::TypeToString(type) << std::endl;
	}
		
	auto call = Nodes::StaticCast<Nodes::Call>(node);
	if (call)
	{
		auto record = sheaf.LookUp(call->name);
		if (record && record->type != type)
		{
			PrintErrorInfo(node->pos, node->line);
			errors << "Function call " << call->name << " does not return of type " << Nodes::TypeToString(type) << std::endl;
		}
	}
		
	//auto arrayExpr = std::static_pointer_cast<Nodes::Call>(node);	
}

void Analyzer::ConsultTable(Nodes::NodePtr node, bool checkGlobalDef)
{	
	InsertGlobalDef(node);
	InsertGlobalDec(node);
	InsertFuncDef(node);
	InsertFuncDec(node);	
	InsertVarDec(node);
	LookUpCall(node);
	LookUpAssignment(node);
	LookUpIdentifier(node);
	if (checkGlobalDef) CheckGlobalDef(node);

	// Function def traversal is done in the InsertFunDef function
	if(!node->IsFamily<Nodes::FunctionDef>())
		Analyse(node, checkGlobalDef);
}

void Analyzer::InsertGlobalDef(Nodes::NodePtr node)
{
	auto globDef = Nodes::StaticCast<Nodes::GlobalDef>(node);
	if (!globDef) return;

	auto record = SymbolTable::Record(false, globDef->var.type, node);
	CheckRedefinition(node, globDef->var.name, sheaf.Insert(globDef->var.name, record));
	globalDefs.push_back(globDef->var.name);
}

void Analyzer::InsertGlobalDec(Nodes::NodePtr node)
{
	auto globDec = Nodes::StaticCast<Nodes::GlobalDec>(node);
	if (!globDec) return;

	auto record = SymbolTable::Record(false, globDec->param.type, node);
	CheckRedefinition(node, globDec->param.name, sheaf.Insert(globDec->param.name, record));		
}

void Analyzer::InsertFuncDef(Nodes::NodePtr node)
{
	auto funDef = Nodes::StaticCast<Nodes::FunctionDef>(node);
	if (!funDef) return;

	if (funDef->header.name.compare("__init") == 0)
	{
		ProcesInitFunc(funDef);
		return;
	}
	
	auto funcRecord = SymbolTable::Record(false, funDef->header.returnType, node);		
	for (auto param : funDef->header.params)	
		funcRecord.funcArgs.push_back(param.type);
	CheckRedefinition(node, funDef->header.name, sheaf.Insert(funDef->header.name, funcRecord));

	//Create new scope
	sheaf.InitializeScope();
	//Add every parameter to the newly created scope
	for (auto param : funDef->header.params)
	{
		auto record = SymbolTable::Record(false, param.type, node);
		CheckRedefinition(node, param.name, sheaf.Insert(param.name, record));
	}
		
	//Add the declarations/definitions inside the function body to the scope
	for (auto child : funDef->children)
	{
		ConsultTable(child);
		TypeCheck(child);
	}

	//Check if the function body contains a return statement
	if (funDef->header.returnType != Nodes::Type::Void)
	{
		if (funDef->children.empty() || !Nodes::StaticCast<Nodes::Return>(funDef->children.back()))
		{
			PrintErrorInfo(node->pos, node->line);
			errors << "Missing return statement" << std::endl;
		}
	}

	sheaf.FinalizeScope();
}

void Analyzer::InsertFuncDec(Nodes::NodePtr node)
{
	auto funDec = Nodes::StaticCast<Nodes::FunctionDec>(node);
	if (!funDec) return;

	auto record = SymbolTable::Record(false, funDec->header.returnType, node);	
	for (auto param : funDec->header.params)
		record.funcArgs.push_back(param.type);
	CheckRedefinition(node, funDec->header.name, sheaf.Insert(funDec->header.name, record));
}

void Analyzer::InsertVarDec(Nodes::NodePtr node)
{
	auto varDec = Nodes::StaticCast<Nodes::VarDec>(node);
	if (!varDec) return;

	auto record = SymbolTable::Record(varDec->immutable, varDec->var.type, node);
	CheckRedefinition(node, varDec->var.name, sheaf.Insert(varDec->var.name, record));
}

void Analyzer::LookUpCall(Nodes::NodePtr node)
{
	auto funCall = Nodes::StaticCast<Nodes::Call>(node);
	if (!funCall) return;

	auto record = sheaf.LookUp(funCall->name);
	if (!record)
	{
		PrintErrorInfo(node->pos, node->line);
		errors << "Unkown function " << funCall->name << std::endl;
	}
	else
	{
		funCall->decPtr = record->decPtr;
	}
}

void Analyzer::LookUpIdentifier(Nodes::NodePtr node)
{	
	auto identifier = Nodes::StaticCast<Nodes::Identifier>(node);
	if (!identifier) return;

	auto record = sheaf.LookUp(identifier->name);
	if (!record)
	{
		PrintErrorInfo(node->pos, node->line);
		errors << "Unkown identifier " << identifier->name << std::endl;
	}
	else
	{
		identifier->defOrDec = record->decPtr;
	}
}

void Analyzer::LookUpAssignment(Nodes::NodePtr node)
{
	auto assignment = Nodes::StaticCast<Nodes::Assignment>(node);
	if (!assignment) return;

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
	auto identifier = Nodes::StaticCast<Nodes::Identifier>(node);
	if (identifier && std::find(globalDefs.begin(), globalDefs.end(), identifier->name) != globalDefs.end())
	{
		PrintErrorInfo(node->pos, node->line);
		errors << "Unkown identifier " << identifier->name << std::endl;
	}
}

void Analyzer::PrintErrorInfo(const int pos, const int line)
{
	errors << "Error at line " << line << " column " << pos << ": ";
}

void Analyzer::CheckRedefinition(Nodes::NodePtr node, std::string name, bool succes)
{
	if (!succes)
	{
		PrintErrorInfo(node->pos, node->line);
		errors << "Redefinition of " << name << std::endl;
	}
}

bool Analyzer::IsNumber(const Nodes::Type type)
{
	return type == Nodes::Type::Int || type == Nodes::Type::Float;
}

bool Analyzer::isBoolOp(const Nodes::NodePtr node)
{
	auto op = Nodes::StaticCast<Nodes::BinaryOp>(node);
	if (!op) return false;

	return op->op == Nodes::Operator::And || op->op == Nodes::Operator::Equal ||
		op->op == Nodes::Operator::LessEqual || op->op == Nodes::Operator::More ||
		op->op == Nodes::Operator::MoreEqual || op->op == Nodes::Operator::Less ||
		op->op == Nodes::Operator::NotEqual || op->op == Nodes::Operator::Or;
}