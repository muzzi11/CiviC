#pragma once

#include "node.h"
#include "symboltable.h"

class Analyzer
{
public:
	Analyzer::Analyzer();
	void Analyse(Nodes::NodePtr root, bool = false);

private:
	void ConsultTable(Nodes::NodePtr node, bool = false);

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
	void CheckGlobalDef(Nodes::NodePtr);

	void TypeCheck(Nodes::NodePtr, Nodes::Type type);
	Nodes::Type GetType(Nodes::NodePtr);

	std::vector<std::string> globalDefs;
	SymbolTable::Sheaf sheaf;
};