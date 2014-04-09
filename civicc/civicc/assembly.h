#pragma once

#include <string>
#include <map>

#include "node.h"


class AssemblyGenerator
{
public:
	std::string Generate(Nodes::NodePtr root);

private:
	struct LocalVarEntry
	{
		int frame, index;
	};

	int labelCounter = 0;

	std::vector<std::string> exports, imports, globals;

	std::map<Nodes::NodePtr, int> importIndex;
	std::map<Nodes::NodePtr, int> globalIndexTable;
	std::map<Nodes::NodePtr, LocalVarEntry> localTable;
	std::map<Nodes::NodePtr, int> assignFrameTable;
	std::map<Nodes::NodePtr, int> idFrameTable;
	std::map<Nodes::NodePtr, int> functionNestingTable;
	std::map<Nodes::NodePtr, int> functionCallTable;

	void BuildTables(Nodes::NodePtr root);

	std::string FunDef(std::shared_ptr<Nodes::FunctionDef> root);
	std::string Assign(std::shared_ptr<Nodes::Assignment> root);
	std::string FunCall(std::shared_ptr<Nodes::Call> call, bool expr);
	std::string Expression(Nodes::NodePtr root);
	std::string Statements(Nodes::NodePtr root);
	std::string ArrayDec(Nodes::NodePtr root);
	std::string IfElse(Nodes::NodePtr root);
	std::string DoWhileLoop(Nodes::NodePtr root);
};
