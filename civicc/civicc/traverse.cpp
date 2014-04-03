#include "traverse.h"


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

std::string TreeToJSON(NodePtr root)
{
	std::string json = "{";
	int prevDepth = 0;

	Traverse(root, prevDepth, [&](NodePtr node, int depth)
	{
		std::string tabs = "\t";

		for(int i = 0; i < depth; ++i) tabs += '\t';

		if(prevDepth < depth) json += " \"children\": {\n";
		else if(prevDepth > depth)
		{
			json.erase(json.length() - 1, 1);
			json += "\n" + tabs + "},\n";
		}
		else json += "\n";

		prevDepth = depth;

		json += tabs;
		json += "\"" + node->FamilyName() + "\": ";
		json += "\"" + node->token.readString + "\",";
	});

	json.replace(json.length() - 1, 1, "\n");
	for(int i = 0; i < prevDepth; ++i)
	{
		for(int k = prevDepth - i - 1; k >= 0; --k)
		{
			json += '\t';
		}
		json += "}\n";
	}

	json += "}";
	return json;
}