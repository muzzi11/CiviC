#include <sstream>

#include "replace_boolops.h"
#include "traverse.h"

using namespace Node;


void ReplaceBooleanOperators(NodePtr root)
{
	Replace<BinaryOp>(root, [](std::shared_ptr<BinaryOp> binaryOp)
	{
		if(binaryOp->op == Operator::And)
		{
			auto ternary = std::make_shared<Ternary>();
			ternary->children.push_back(binaryOp->children[0]);
			ternary->children.push_back(binaryOp->children[1]);
			ternary->children.push_back(std::make_shared<Literal>(false));
			return std::static_pointer_cast<BaseNode>(ternary);
		}
		else if(binaryOp->op == Operator::Or)
		{
			auto ternary = std::make_shared<Ternary>();
			ternary->children.push_back(binaryOp->children[0]);
			ternary->children.push_back(std::make_shared<Literal>(true));
			ternary->children.push_back(binaryOp->children[1]);
			return std::static_pointer_cast<BaseNode>(ternary);
		}

		return std::static_pointer_cast<BaseNode>(binaryOp);
	});
}