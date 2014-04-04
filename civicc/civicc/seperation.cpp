#include <algorithm>

#include "seperation.h"
#include "traverse.h"

using namespace Node;


void SeperateVarDecFromInit(NodePtr root)
{
	Traverse<VarDec>(root, [](std::shared_ptr<VarDec> varDec, NodePtr parent)
	{
		if(varDec->children.empty()) return true;

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