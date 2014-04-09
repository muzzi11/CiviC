#include <iostream>
#include <algorithm>

#include "analysis.h"
#include "symboltable.h"
#include "traverse.h"

Analyzer::Analyzer()
{
	sheaf.InitializeScope();
}

std::string Analyzer::Analyse(Nodes::NodePtr root)
{
	if (!root->children.empty())
	{
		for (auto child : root->children)
		{
			InsertGlobalDef(child);
			InsertGlobalDec(child);
		}

		BuildTable(root);

		CheckReturnStatements(root);
	}
	return errors.str();
}

void Analyzer::BuildTable(Nodes::NodePtr root)
{		
	for (auto child : root->children)
	{
		InsertFuncDec(child);
		InsertFuncDef(child);
	}

	for (auto child : root->children)
	{
		InsertFuncBody(child);
		InsertVarDec(child);

		LookUpCall(child);
		LookUpAssignment(child);
		LookUpIdentifier(child);

		TypeCheck(child);

		if (!child->IsFamily<Nodes::FunctionDef>())
			BuildTable(child);
	}
}

void Analyzer::InsertFuncDec(Nodes::NodePtr node)
{
	auto funDec = Nodes::StaticCast<Nodes::FunctionDec>(node);
	if (!funDec) return;

	auto record = SymbolTable::Record(false, funDec->header.returnType, node);
	record.params = funDec->header.params;
	CheckRedefinition(node, funDec->header.name, sheaf.Insert(funDec->header.name, record));
}

void Analyzer::InsertFuncDef(Nodes::NodePtr node)
{
	auto funDef = Nodes::StaticCast<Nodes::FunctionDef>(node);
	if (!funDef) return;

	if (funDef->header.name.compare("__init") == 0) return InsertInitFunc(funDef);
		
	auto funcRecord = SymbolTable::Record(false, funDef->header.returnType, node);
	funcRecord.params = funDef->header.params;
	CheckRedefinition(node, funDef->header.name, sheaf.Insert(funDef->header.name, funcRecord));
}

void Analyzer::InsertFuncBody(Nodes::NodePtr node)
{
	auto funDef = Nodes::StaticCast<Nodes::FunctionDef>(node);
	if (!funDef) return;

	sheaf.InitializeScope();
	//Add every parameter to the newly created scope
	for (auto param : funDef->header.params)
	{
		auto record = SymbolTable::Record(false, param.type, node);
		CheckRedefinition(node, param.name, sheaf.Insert(param.name, record));

		//Add array dimension variables to the table
		if (param.dim.size() > 0)
		{
			for (auto dim : param.dim)
			{
				auto record = SymbolTable::Record(false, Nodes::Type::Int, node);
				CheckRedefinition(node, dim, sheaf.Insert(dim, record));
			}
		}
	}

	BuildTable(node);
		
	sheaf.FinalizeScope();
}

void Analyzer::InsertInitFunc(std::shared_ptr<Nodes::FunctionDef> funcDef)
{
	auto funcRecord = SymbolTable::Record(false, funcDef->header.returnType, funcDef);

	for (auto child : funcDef->children)
	{
		BuildTable(child);
		CheckGlobalDef(child);
	}
}

void Analyzer::InsertGlobalDef(Nodes::NodePtr node)
{
	auto globDef = Nodes::StaticCast<Nodes::GlobalDef>(node);
	if (!globDef) return;

	auto record = SymbolTable::Record(false, globDef->var.type, node);

	if (globDef->var.array)
	{
		CheckArrayType(node, Nodes::Type::Int);
		for (auto child : globDef->children[0]->children)
		{
			auto lit = Nodes::StaticCast<Nodes::Literal>(child);
			if (!lit)
			{
				record.arrayDimensions.clear();
				break;
			}
			record.arrayDimensions.push_back(lit->intValue);
		}
	}

	CheckRedefinition(node, globDef->var.name, sheaf.Insert(globDef->var.name, record));
	globalDefs.push_back(globDef->var.name);
}

void Analyzer::InsertGlobalDec(Nodes::NodePtr node)
{
	auto globDec = Nodes::StaticCast<Nodes::GlobalDec>(node);
	if (!globDec) return;

	auto record = SymbolTable::Record(false, globDec->param.type, node);
	record.dim = globDec->param.dim;
	CheckRedefinition(node, globDec->param.name, sheaf.Insert(globDec->param.name, record));
}

void Analyzer::InsertVarDec(Nodes::NodePtr node)
{
	auto varDec = Nodes::StaticCast<Nodes::VarDec>(node);
	if (!varDec) return;

	auto record = SymbolTable::Record(varDec->immutable, varDec->var.type, node);

	if (varDec->var.array)
	{
		CheckArrayType(node, Nodes::Type::Int);
		for (auto child : varDec->children[0]->children)
		{
			auto lit = Nodes::StaticCast<Nodes::Literal>(child);
			if (!lit)
			{
				record.arrayDimensions.clear();
				break;
			}
			record.arrayDimensions.push_back(lit->intValue);
		}
	}
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
		funCall->dec = record->decPtr;
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
		identifier->dec = record->decPtr;
		identifier->type = record->type;
	}
}

void Analyzer::LookUpAssignment(Nodes::NodePtr node)
{
	auto assignment = Nodes::StaticCast<Nodes::Assignment>(node);
	if (!assignment) return;

	auto record = sheaf.LookUp(assignment->name);
	if (record)
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

void Analyzer::TypeCheck(Nodes::NodePtr node)
{
	TypeCheckAssigment(node);
	TypeCheckUnary(node);
	TypeCheckBinOp(node);
	TypeCheckFuncArgs(node);
	TypeCheckFuncReturn(node);
	TypeCheckCast(node);
	TypeCheckConditional(node);
}

void Analyzer::TypeCheckAssigment(Nodes::NodePtr node)
{
	auto ass = Nodes::StaticCast<Nodes::Assignment>(node);
	if (!ass) return;

	auto record = sheaf.LookUp(ass->name);
	if (record)
	{
		//Check assigment of immutables (for loop counters)
		if (record->immutable && record->initialized)
		{
			PrintErrorInfo(node->pos, node->line);
			errors << "The identifier " << ass->name << " is immutable" << std::endl;
		}
		else
		{
			Nodes::NodePtr arrayExpr = nullptr;
			//Check array on the right hand side of the assignment, compare dimensions of assignment with declaration
			arrayExpr = Nodes::StaticCast<Nodes::ArrayExpr>(ass->children[0]);
			if (arrayExpr && ass->children.size() == 1)
			{
				CheckArrayDimensions(node, record->arrayDimensions);
				CheckArrayType(node, record->type);
			}
			//Check array on the left hand side, type always int
			else if (arrayExpr)
			{
				CheckArrayDimensions(node, record->arrayDimensions);
				CheckArrayType(node, Nodes::Type::Int);
			}
			else
			{
				record->initialized = true;
				for (auto child : ass->children)
				{
					TypeCheck(child, record->type);
				}
			}
		}
	}
}

void Analyzer::TypeCheckBinOp(Nodes::NodePtr node)
{
	auto binOp = Nodes::StaticCast<Nodes::BinaryOp>(node);
	if (!binOp) return;

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
	else if (IsNumberComp(node) && (!IsNumber(leftType) || !IsNumber(rightType)))
	{
		PrintErrorInfo(node->pos, node->line);
		errors << "Both operands of this comparison operator must be of type int or float" << std::endl;
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
		if (record->params.size() != funcCall->children.size())
		{
			PrintErrorInfo(node->pos, node->line);
			errors << "Arg count does not match function definition" << std::endl;
			return;
		}
		for (size_t i = 0; i < funcCall->children.size(); ++i)
		{
			auto arg = funcCall->children[i];
			auto param = record->params[i];
			TypeCheck(arg, param.type);

			auto id = Nodes::StaticCast<Nodes::Identifier>(arg);
			if (!id) continue;

			auto record = sheaf.LookUp(id->name);
			if (record->dim.size() > 0)
			{
				for (size_t i = 0; i < record->dim.size(); ++i)
				{
					if (record->dim[i].compare(param.dim[i]) != 0)
					{
						PrintErrorInfo(node->pos, node->line);
						errors << "Incompatible array dimensions" << std::endl;
					}
				}
			}
		}
	}
}

void Analyzer::TypeCheckFuncReturn(Nodes::NodePtr node)
{
	auto returnVal = Nodes::StaticCast<Nodes::Return>(node);
	if (!returnVal) return;

	auto record = sheaf.LookUp(returnVal->functionName);
	if (record)
	{
		for (auto child : returnVal->children)
			TypeCheck(child, record->type);

		returnVal->type = GetType(returnVal);
	}
	returnVal->type = GetType(returnVal);
}

void Analyzer::TypeCheckCast(Nodes::NodePtr node)
{
	auto cast = Nodes::StaticCast<Nodes::Cast>(node);
	if (cast) cast->castFrom = GetType(cast->children[0]);
}

void Analyzer::TypeCheckConditional(Nodes::NodePtr node)
{
	auto whileLoop = Nodes::StaticCast<Nodes::While>(node);
	if (whileLoop) TypeCheckExpression(whileLoop->children.front(), Nodes::Type::Bool);

	auto doWhile = Nodes::StaticCast<Nodes::DoWhile>(node);
	if (doWhile) TypeCheckExpression(doWhile->children.back(), Nodes::Type::Bool);

	auto ifStatement = Nodes::StaticCast<Nodes::If>(node);
	if (ifStatement) TypeCheckExpression(ifStatement->children.front(), Nodes::Type::Bool);
}

void Analyzer::TypeCheckExpression(Nodes::NodePtr node, Nodes::Type type)
{
	if (node->children.empty()) TypeCheck(node, type);

	for (auto child : node->children)
	{
		auto lit = Nodes::StaticCast<Nodes::Literal>(node);
		if (lit && lit->type != type)
		{
			PrintErrorInfo(node->pos, node->line);
			errors << "Literal is not of type " << Nodes::TypeToString(type) << std::endl;
			return;
		}

		auto binOp = Nodes::StaticCast<Nodes::BinaryOp>(node);
		if (binOp && GetType(node) != type)
		{			
			if (type == Nodes::Type::Bool && isBoolOp(node)) return;
			PrintErrorInfo(node->pos, node->line);
			errors << "The expression is not of type " << Nodes::TypeToString(type) << std::endl;
			return;
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
	}
}

void Analyzer::TypeCheck(Nodes::NodePtr node, Nodes::Type type)
{
	auto binOp = Nodes::StaticCast<Nodes::BinaryOp>(node);
	if (binOp)
	{
		if (isBoolOp(binOp))
		{
			if (type != Nodes::Type::Bool)
			{
				PrintErrorInfo(node->pos, node->line);
				errors << "Inappropiate use of boolean operator " << std::endl;
			}
		}
		else
		{
			for (auto child : binOp->children)
				TypeCheck(child, type);
		}
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
}

void Analyzer::CheckGlobalDef(Nodes::NodePtr node)
{
	auto ass = Nodes::StaticCast<Nodes::Assignment>(node);
	if (!ass) return;

	TraverseBreadth<Nodes::Identifier>(node, [&](std::shared_ptr<Nodes::Identifier> id, Nodes::NodePtr)
	{
		//The identifier in the right hand side of the assignment has to be declared before, the one on the left.
		if (std::find(globalDefs.begin(), globalDefs.end(), ass->name) < (std::find(globalDefs.begin(), globalDefs.end(), id->name)))
		{
			PrintErrorInfo(node->pos, node->line);
			errors << "Unkown identifier " << id->name << std::endl;
		}
	});
}

void Analyzer::CheckRedefinition(Nodes::NodePtr node, std::string name, bool succes)
{
	if (!succes)
	{
		PrintErrorInfo(node->pos, node->line);
		errors << "Redefinition of " << name << std::endl;
	}
}

void Analyzer::CheckArrayDimensions(Nodes::NodePtr node, std::vector<int> dimensions)
{
	if (dimensions.size() == 0) return;

	Nodes::NodePtr cur = node;
	int level = 0;
	bool succes = true;
	TraverseBreadth<Nodes::ArrayExpr>(node, [&](std::shared_ptr<Nodes::ArrayExpr> arrayExpr, Nodes::NodePtr parent)
	{
		if (parent != cur)
		{
			cur = parent;
			++level;
		}
		succes = succes && (dimensions[level] && dimensions[level] == (int)arrayExpr->children.size());
	});

	if (!succes)
	{
		PrintErrorInfo(node->pos, node->line);
		errors << "Incompatible array dimensions" << std::endl;
	}
}

void Analyzer::CheckArrayType(Nodes::NodePtr node, Nodes::Type type)
{
	TraverseBreadth<Nodes::ArrayExpr>(node, [&](std::shared_ptr<Nodes::ArrayExpr> arrayExpr, Nodes::NodePtr)
	{
		for (auto child : arrayExpr->children)
			TypeCheck(child, type);
	});
}

void Analyzer::CheckReturnStatements(Nodes::NodePtr root)
{
	TraverseBreadth<Nodes::FunctionDef>(root, [&](std::shared_ptr<Nodes::FunctionDef> funDef, Nodes::NodePtr)
	{
		if (funDef->header.returnType != Nodes::Type::Void)
		{
			if (funDef->children.empty() || !Nodes::StaticCast<Nodes::Return>(funDef->children.back()))
			{
				PrintErrorInfo(funDef->pos, funDef->line);
				errors << "Missing return statement" << std::endl;
			}
		}
	});	
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
		if (type != Nodes::Type::None) return type;
	}
	return Nodes::Type::None;
}


void Analyzer::PrintErrorInfo(const int pos, const int line)
{
	errors << "Error at line " << line << " column " << pos << ": ";
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

bool Analyzer::IsNumberComp(const Nodes::NodePtr node)
{
	auto op = Nodes::StaticCast<Nodes::BinaryOp>(node);
	if (!op) return false;

	return op->op == Nodes::Operator::LessEqual || op->op == Nodes::Operator::More ||
		op->op == Nodes::Operator::MoreEqual || op->op == Nodes::Operator::Less;
}
