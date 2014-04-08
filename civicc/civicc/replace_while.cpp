#include "replace_while.h"
#include "traverse.h"

using namespace Nodes;


void ReplaceWhileLoops(NodePtr root)
{
	Replace<While>(root, [](std::shared_ptr<While> node)
	{
		auto ifStatement = std::make_shared<If>();
		auto doWhile = std::make_shared<DoWhile>();

		ifStatement->children.push_back(node->children[0]);
		ifStatement->children.push_back(doWhile);

		for(size_t i = 1; i < node->children.size(); ++i) doWhile->children.push_back(node->children[i]);
		doWhile->children.push_back(node->children[0]);

		return ifStatement;
	});
}