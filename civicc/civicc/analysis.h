#pragma once

#include <sstream>
#include <string>

#include "node.h"
#include "symboltable.h"

class Analyzer
{
public:
	Analyzer::Analyzer();
	std::string Analyse(Nodes::NodePtr root);

private:
	void ConsultTable(Nodes::NodePtr node);

	void InsertGlobalDef(Nodes::NodePtr node);
	void InsertGlobalDec(Nodes::NodePtr node);
	void InsertFuncDef(Nodes::NodePtr node);
	void InsertFuncDec(Nodes::NodePtr node);
	void InsertVarDec(Nodes::NodePtr node);

	void LookUpCall(Nodes::NodePtr node);
	void LookUpIdentifier(Nodes::NodePtr node);
	void LookUpAssignment(Nodes::NodePtr node);
	void ProcesInitFunc(std::shared_ptr<Nodes::FunctionDef> funcDef);

	void TypeCheck(Nodes::NodePtr);
	void TypeCheckAssigment(Nodes::NodePtr);
	void TypeCheckBinOp(Nodes::NodePtr);
	void TypeCheckFuncArgs(Nodes::NodePtr);
	void TypeCheckFuncReturn(Nodes::NodePtr);
	void TypeCheckUnary(Nodes::NodePtr);
	void TypeCheckCast(Nodes::NodePtr);

	void CheckGlobalDef(Nodes::NodePtr);
	void CheckRedefinition(Nodes::NodePtr, std::string name, bool result);
	void CheckArrayDimensions(Nodes::NodePtr node, std::vector<int>);
	void CheckArrayType(Nodes::NodePtr, Nodes::Type);

	void TypeCheck(Nodes::NodePtr, Nodes::Type type);
	Nodes::Type GetType(Nodes::NodePtr);

	void PrintErrorInfo(const int pos, const int line);
	bool IsNumber(const Nodes::Type);
	bool isBoolOp(const Nodes::NodePtr);

	std::vector<std::string> globalDefs;
	SymbolTable::Sheaf sheaf;
	std::stringstream errors;
};