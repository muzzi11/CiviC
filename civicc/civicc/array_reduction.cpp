#include "array_reduction.h"
#include "traverse.h"

using namespace Nodes;


void ReduceArrayExpressions(NodePtr root)
{
	/*
	TraverseBreadth<VarDec>(root, [](std::shared_ptr<VarDec> node, NodePtr)
	{
		for(auto child : node->children)
		{
			auto expr = StaticCast<ArrayExpr>(child);
			if(expr) expr->type = node->var.type;
		}
	});

	TraverseBreadth<GlobalDef>(root, [](std::shared_ptr<GlobalDef> node, NodePtr)
	{
		for(auto child : node->children)
		{
			auto expr = StaticCast<ArrayExpr>(child);
			if(expr) expr->type = node->var.type;
		}
	});

	TraverseBreadth<Assignment>(root, [](std::shared_ptr<Assignment> node, NodePtr)
	{
		for(auto child : node->children)
		{
			auto expr = StaticCast<ArrayExpr>(child);
			if(expr) expr->type = node->type;
		}
	});

	TraverseBreadth<ArrayExpr>(root, [](std::shared_ptr<ArrayExpr> node, NodePtr parent)
	{
		TraverseDepth<ArrayExpr>(node, [&](std::shared_ptr<ArrayExpr> innerNode, NodePtr)
		{
			innerNode->type = node->type;
		});
	});
	*/

	Replace<ArrayExpr>(root, [](std::shared_ptr<ArrayExpr> node) -> NodePtr
	{
		if(node->children.size() > 1)
		{
			auto expr = std::make_shared<BinaryOp>(Operator::Multiply);
			NodePtr cur = expr;

			for(size_t i = 0; i < node->children.size(); i += 2)
			{
				auto mult = std::make_shared<BinaryOp>(Operator::Multiply);
				mult->children.push_back(node->children[i]);
				mult->children.push_back(node->children[i + 1]);
				cur->children.push_back(mult);
				if(cur->children.size() == 2) cur = mult->children[1];
			}

			if(node->children.size() % 2 != 0)
			{
				auto mult = std::make_shared<BinaryOp>(Operator::Multiply);
				mult->children.push_back(expr);
				mult->children.push_back(node->children.back());
				expr = mult;
			}

			return expr;
		}
		else return node->children[0];
	});
}