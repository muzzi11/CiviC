#include <iostream>

#include "nested_func_renaming.h"
#include "traverse.h"

using namespace Nodes;

void RenameNestedFunctions(NodePtr root)
{
	for (auto child : root->children)
	{
		auto topFuncDef = StaticCast<FunctionDef>(child);
		if (!topFuncDef) continue;

		ReplaceBreadth<FunctionDef>(child, [&](std::shared_ptr<FunctionDef> funcDef, std::shared_ptr<FunctionDef> parent) -> NodePtr
		{
			auto newName = parent->header.name + "__" + funcDef->header.name;
			
			Replace<Call>(child, [&](std::shared_ptr<Call> call) -> NodePtr
			{
				if (call->name.compare(funcDef->header.name) == 0)
					call->name = newName;
				return call;
			});
			funcDef->header.name = newName;
			return funcDef;
		});				
	}		
}