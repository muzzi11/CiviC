#pragma once

#include "node.h"
#include "symboltable.h"

class Analyzer
{
public:
	Analyzer::Analyzer();
	void Analyse(Node::NodePtr root);

private:
	void ConsultTable(Node::NodePtr node);

	void InsertGlobalDef(Node::NodePtr node);
	void InsertGlobalDec(Node::NodePtr node);
	void InsertFuncDef(Node::NodePtr node);
	void InsertFuncDec(Node::NodePtr node);
	void InsertVarDec(Node::NodePtr node);
	void LookUpCall(Node::NodePtr node);
	void LookUpIdentifier(Node::NodePtr node);

	void TypeCheck(Node::NodePtr);

	SymbolTable::Sheaf sheaf;
};