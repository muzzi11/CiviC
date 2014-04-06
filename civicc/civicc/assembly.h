#pragma once

#include <string>
#include <unordered_map>

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

	std::unordered_map<Nodes::NodePtr, int> globalIndexTable;
	std::unordered_map<Nodes::NodePtr, LocalVarEntry> localTable;
	std::unordered_map<Nodes::NodePtr, int> assignFrameTable;
	std::unordered_map<Nodes::NodePtr, int> functionNestingTable;

	void BuildTables(Nodes::NodePtr root);

	std::string FunDef(std::shared_ptr<Nodes::FunctionDef> root);
	std::string Assign(std::shared_ptr<Nodes::Assignment> root);
	std::string Expression(Nodes::NodePtr root);
};