#include <sstream>

#include "replace_boolops.h"
#include "traverse.h"

using namespace Node;


void ReplaceBooleanOperators(NodePtr root)
{
	Replace<BinaryOp>(root, [](std::shared_ptr<BinaryOp> binaryOp) -> NodePtr
	{
		if(binaryOp->op == Operator::And)
		{
			auto ternary = std::make_shared<Ternary>();

			ternary->children.push_back(binaryOp->children[0]);
			ternary->children.push_back(binaryOp->children[1]);
			ternary->children.push_back(std::make_shared<Literal>(false));

			return ternary;
		}
		else if(binaryOp->op == Operator::Or)
		{
			auto ternary = std::make_shared<Ternary>();

			ternary->children.push_back(binaryOp->children[0]);
			ternary->children.push_back(std::make_shared<Literal>(true));
			ternary->children.push_back(binaryOp->children[1]);

			return ternary;
		}
		else if(binaryOp->op == Operator::Multiply && binaryOp->type == Type::Bool)
		{
			auto first = std::make_shared<Ternary>();
			auto second = std::make_shared<Ternary>();

			first->children.push_back(binaryOp->children[0]);
			first->children.push_back(binaryOp->children[1]);
			second->children.push_back(binaryOp->children[1]);
			second->children.push_back(std::make_shared<Literal>(false));
			second->children.push_back(std::make_shared<Literal>(false));
			first->children.push_back(second);

			return first;
		}
		else if(binaryOp->op == Operator::Add && binaryOp->type == Type::Bool)
		{
			auto first = std::make_shared<Ternary>();
			auto second = std::make_shared<Ternary>();

			first->children.push_back(binaryOp->children[0]);
			second->children.push_back(binaryOp->children[1]);
			second->children.push_back(std::make_shared<Literal>(true));
			second->children.push_back(std::make_shared<Literal>(true));
			first->children.push_back(second);
			first->children.push_back(binaryOp->children[1]);

			return first;
		}

		return binaryOp;
	});

	Replace<Cast>(root, [](std::shared_ptr<Cast> cast) -> NodePtr
	{
		if(cast->type == Type::Bool)
		{

		}

		return cast;
	});
}