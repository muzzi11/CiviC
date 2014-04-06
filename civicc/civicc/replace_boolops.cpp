#include <sstream>

#include "replace_boolops.h"
#include "traverse.h"

using namespace Nodes;

void ReplaceBooleanOperators(NodePtr root)
{
	Replace<BinaryOp>(root, [](std::shared_ptr<BinaryOp> binaryOp) -> NodePtr
	{
		if(binaryOp->op == Operator::And)
		{
			// a && b = a ? b : false
			auto ternary = std::make_shared<Ternary>();

			ternary->children.push_back(binaryOp->children[0]);
			ternary->children.push_back(binaryOp->children[1]);
			ternary->children.push_back(std::make_shared<Literal>(false));

			return ternary;
		}
		else if(binaryOp->op == Operator::Or)
		{
			// a || b = a ? true : b
			auto ternary = std::make_shared<Ternary>();

			ternary->children.push_back(binaryOp->children[0]);
			ternary->children.push_back(std::make_shared<Literal>(true));
			ternary->children.push_back(binaryOp->children[1]);

			return ternary;
		}
		else if(binaryOp->op == Operator::Multiply && binaryOp->type == Type::Bool)
		{
			// a * b = a ? b : (b ? false : false)
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
			// a + b = a ? (b ? true : true) : b
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
			if(cast->castFrom == Type::Int)
			{
				auto unequal = std::make_shared<BinaryOp>(Operator::NotEqual);
				unequal->children.push_back(cast->children[0]);
				unequal->children.push_back(std::make_shared<Literal>(0));
				return unequal;
			}
			else if(cast->castFrom == Type::Float)
			{
				auto unequal = std::make_shared<BinaryOp>(Operator::NotEqual);
				unequal->children.push_back(cast->children[0]);
				unequal->children.push_back(std::make_shared<Literal>(0.0f));
				return unequal;
			}
		}

		if(cast->castFrom == Type::Bool)
		{
			auto ternary = std::make_shared<Ternary>();
			ternary->children.push_back(cast->children[0]);

			if(cast->type == Type::Int)
			{
				ternary->children.push_back(std::make_shared<Literal>(1));
				ternary->children.push_back(std::make_shared<Literal>(0));
			}
			else
			{
				ternary->children.push_back(std::make_shared<Literal>(1.0f));
				ternary->children.push_back(std::make_shared<Literal>(0.0f));
			}

			return ternary;
		}

		return cast;
	});
}