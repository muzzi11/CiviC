#include "traverse.h"

using namespace Node;


void Traverse(NodePtr root, int depth, std::function<void(NodePtr, int)> func)
{
	if(root)
	{
		func(root, depth);
		for(auto child : root->children)
		{
			Traverse(child, depth + 1, func);
		}
	}
}

std::string ToJSON(NodePtr root, int depth)
{
	std::string str;

	if(root)
	{
		std::string tabs(depth + 1, '\t');

		str += tabs + "\"" + root->FamilyName() + "\": \"" + root->ToString() + "\"";
		if(!root->children.empty())
		{
			str += ",\n" + tabs + "\"children\": {";
			for(auto child : root->children)
			{
				str += '\n' + ToJSON(child, depth + 1) + ",";
			}
			str.pop_back();
			str += "\n" + tabs + "}";
		}
	}

	return str;
}

std::string TreeToJSON(NodePtr root)
{
	return "{\n" + ToJSON(root, 0) + "\n}";
}
