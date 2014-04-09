#include "nested_func_renaming.h"
#include "traverse.h"

using namespace Nodes;

void RenameNestedFunctions(NodePtr root)
{
	TraverseBreadth<FunctionDef>(root, [](std::shared_ptr<FunctionDef> funcDef, NodePtr parent)
	{
		auto outerFun = StaticCast<FunctionDef>(parent);
		if(!outerFun) return;

		funcDef->header.name = "_F_" + outerFun->header.name + "_" + funcDef->header.name;
	});

	TraverseBreadth<Call>(root, [](std::shared_ptr<Call> call, NodePtr parent)
	{
		auto funDef = StaticCast<FunctionDef>(call->dec);
		if(funDef) call->name = funDef->header.name;
	});
}