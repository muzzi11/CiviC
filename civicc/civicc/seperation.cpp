#include <algorithm>
#include <vector>
#include <map>
#include <sstream>

#include "seperation.h"
#include "traverse.h"

using namespace Node;


void SeperateVarDecFromInit(NodePtr root)
{
	TraverseBreadth<VarDec>(root, [](std::shared_ptr<VarDec> varDec, NodePtr parent)
	{
		if(!varDec->HasAssignment()) return true;

		auto& list = parent->children;
		auto it = std::find(list.begin(), list.end(), varDec);
		if(it != list.end())
		{
			auto assignment = std::make_shared<Assignment>(varDec->var.name);
			assignment->children.push_back(varDec->children.back());
			list.insert(++it, assignment);
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

	TraverseBreadth<GlobalDef>(root, [&](std::shared_ptr<GlobalDef> globalDef, NodePtr parent)
	{
		if(!globalDef->HasAssignment()) return true;

		auto assignment = std::make_shared<Assignment>(globalDef->var.name);
		assignment->children.push_back(globalDef->children.back());
		init->children.push_back(assignment);
		globalDef->children.pop_back();

		return true;
	});

	if(!init->children.empty()) root->children.push_back(init);
}

void ReplaceNamesInFor(NodePtr root)
{
	int counter = 0;

	TraverseDepth<For>(root, [&](std::shared_ptr<For> forLoop, NodePtr)
	{
		auto it = std::static_pointer_cast<VarDec>(forLoop->children[0]);
		auto lower = std::static_pointer_cast<Assignment>(forLoop->children[1]);
		std::stringstream newName;
		newName << "_L" << counter++ << it->var.name;

		for(size_t i = 4; i < forLoop->children.size(); ++i)
		{
			TraverseBreadth<Assignment>(forLoop->children[i], [&](std::shared_ptr<Assignment> assignment, NodePtr)
			{
				if(assignment->name == it->var.name) assignment->name = newName.str();
				return true;
			});

			TraverseBreadth<Identifier>(forLoop->children[i], [&](std::shared_ptr<Identifier> id, NodePtr)
			{
				if(id->name == it->var.name) id->name = newName.str();
				return true;
			});
		}

		it->var.name = newName.str();
		lower->name = newName.str();

		return true;
	});
}

void SeperateForLoopInduction(NodePtr root)
{
	std::stringstream sstream;
	std::map<NodePtr, std::vector<NodePtr>> map;

	ReplaceNamesInFor(root);

	TraverseDepth<For>(root, [&](std::shared_ptr<For> forLoop, NodePtr parent)
	{
		auto lowerVar = std::static_pointer_cast<VarDec>(forLoop->children[0]);
		auto lowerAss = std::static_pointer_cast<Assignment>(forLoop->children[1]);
		auto upperVar = std::make_shared<VarDec>();
		auto upperAss = std::make_shared<Assignment>("");
		auto stepVar = std::make_shared<VarDec>();
		auto stepAss = std::make_shared<Assignment>("");

		lowerVar->immutable = true;
		upperVar->immutable = true;
		upperVar->var.type = Type::Int;
		upperVar->var.name = "_U" + lowerVar->var.name;
		upperAss->children.push_back(forLoop->children[2]);
		stepVar->immutable = true;
		stepVar->var.type = Type::Int;
		stepVar->var.name = "_S" + lowerVar->var.name;
		stepAss->children.push_back(forLoop->children[3]);

		map[parent].push_back(forLoop);
		map[parent].push_back(lowerVar);
		map[parent].push_back(lowerAss);
		map[parent].push_back(upperVar);
		map[parent].push_back(upperAss);
		map[parent].push_back(stepVar);
		map[parent].push_back(stepAss);

		return true;
	});

	for(auto pair : map)
	{
		NodePtr parent = pair.first;
		std::vector<NodePtr>& newInserts = pair.second;

		for(size_t i = 0; i < newInserts.size(); i += 7)
		{
			NodePtr forLoop = newInserts[i];
			auto it = std::find(parent->children.begin(), parent->children.end(), forLoop);
			parent->children.insert(it, newInserts.begin() + i + 1, newInserts.begin() + i + 7);
			forLoop->children.erase(forLoop->children.begin(), forLoop->children.begin() + 4);
		}
	}
}

void SeperateDecAndInit(NodePtr root)
{
	SeperateVarDecFromInit(root);
	SeperateGlobalDefFromInit(root);
	SeperateForLoopInduction(root);
}