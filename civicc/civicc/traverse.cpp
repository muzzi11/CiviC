#include "traverse.h"

using namespace Nodes;


void TraverseBreadth(NodePtr root, std::function<bool(NodePtr, NodePtr)> func, NodePtr parent)
{
	if(root)
	{
		if(parent == nullptr && !func(root, parent)) return;
		size_t i = 0;
		while(i < root->children.size())
		{
			if(!func(root->children[i++], root)) return;
		}
		i = 0;
		while(i < root->children.size()) TraverseBreadth(root->children[i++], func, root);
	}
}

void TraverseDepth(NodePtr root, std::function<bool(NodePtr, NodePtr)> func, NodePtr parent)
{
	if(root)
	{
		size_t i = 0;
		while(i < root->children.size()) TraverseDepth(root->children[i++], func, root);
		if(!func(root, parent)) return;
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
