#include "replace_loops.h"
#include "traverse.h"

using namespace Nodes;


void ReplaceForLoops(NodePtr root)
{
	Replace<For>(root, [](std::shared_ptr<For> forLoop)
	{
		auto whileLoop = std::make_shared<While>();
		auto condition = std::make_shared<Ternary>();
		auto stepId = std::make_shared<Identifier>(forLoop->step->var.name);
		auto lowerId = std::make_shared<Identifier>(forLoop->lower->var.name);
		auto upperId = std::make_shared<Identifier>(forLoop->upper->var.name);
		auto stepOp = std::make_shared<BinaryOp>(Operator::More);
		auto lessOp = std::make_shared<BinaryOp>(Operator::Less);
		auto moreOp = std::make_shared<BinaryOp>(Operator::More);
		auto assign = std::make_shared<Assignment>(forLoop->lower->var.name);
		auto increment = std::make_shared<BinaryOp>(Operator::Add);

		whileLoop->children.push_back(condition);
		whileLoop->children.insert(whileLoop->children.end(), forLoop->children.begin(), forLoop->children.end());
		whileLoop->children.push_back(assign);

		assign->type = increment->type = Type::Int;
		assign->dec = forLoop->lower;
		assign->children.push_back(increment);
		increment->children.push_back(lowerId);
		increment->children.push_back(upperId);

		stepId->type = lowerId->type = upperId->type = Type::Int;
		stepId->dec = forLoop->step;
		lowerId->dec = forLoop->lower;
		upperId->dec = forLoop->upper;

		stepOp->type = lessOp->type = moreOp->type = Type::Int;
		stepOp->children.push_back(stepId);
		stepOp->children.push_back(std::make_shared<Literal>(0));

		lessOp->children.push_back(lowerId);
		lessOp->children.push_back(upperId);

		moreOp->children.push_back(lowerId);
		moreOp->children.push_back(upperId);

		condition->children.push_back(stepOp);
		condition->children.push_back(lessOp);
		condition->children.push_back(moreOp);

		return whileLoop;
	});
}

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

void ReplaceLoops(NodePtr root)
{
	ReplaceForLoops(root);
	ReplaceWhileLoops(root);
}