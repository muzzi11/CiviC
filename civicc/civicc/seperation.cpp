#include <algorithm>
#include <vector>
#include <map>
#include <sstream>

#include "seperation.h"
#include "traverse.h"

using namespace Node;


void SeperateVarDecFromInit(NodePtr root)
{
	Traverse<VarDec>(root, [](std::shared_ptr<VarDec> varDec, NodePtr parent)
	{
		if(!varDec->HasAssignment()) return true;

		auto& list = parent->children;
		auto it = std::find(list.begin(), list.end(), varDec);
		if(it != list.end())
		{
			auto assign = std::make_shared<Assignment>(varDec->var.name);
			assign->children.push_back(varDec->children.back());
			list.insert(++it, assign);
			varDec->children.pop_back();
		}

		return true;
	});
}

void SeperateGlobalDefFromInit(NodePtr root)
{
	auto init = std::make_shared<FunctionDef>();
	init->exp = false;
	init->header.name = "__init";
	init->header.returnType = Type::Void;

	Traverse<GlobalDef>(root, [&](std::shared_ptr<GlobalDef> globalDef, NodePtr parent)
	{
		if(!globalDef->HasAssignment()) return true;

		init->children.push_back(globalDef->children.back());
		globalDef->children.pop_back();

		return true;
	});

	if(!init->children.empty()) root->children.push_back(init);
}

void ReplaceNamesInFor(NodePtr root)
{
	for(auto child : root->children)
	{

	}
}

void SeperateForLoopInduction(NodePtr root)
{
	std::stringstream sstream;
	std::map<NodePtr, std::vector<NodePtr>> map;

	Traverse<For>(root, [&](std::shared_ptr<For> forLoop, NodePtr parent)
	{
		auto it = std::static_pointer_cast<VarDec>(forLoop->children[0]);
		auto upperVar = std::make_shared<VarDec>();
		auto upperAss = std::make_shared<Assignment>("");
		auto stepVar = std::make_shared<VarDec>();
		auto stepAss = std::make_shared<Assignment>("");

		it->immutable = true;
		upperVar->immutable = true;
		upperVar->var.type = Type::Int;
		upperVar->var.name = "_U" + it->var.name;
		upperAss->children.push_back(forLoop->children[2]);
		stepVar->immutable = true;
		stepVar->var.type = Type::Int;
		stepVar->var.name = "_S" + it->var.name;
		stepAss->children.push_back(forLoop->children[3]);

		return true;
	});
}