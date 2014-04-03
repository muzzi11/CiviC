#pragma once

#include <string>
#include <memory>
#include <functional>

#include "node.h"


template<class T>
void Traverse(NodePtr root, std::function<void(std::shared_ptr<T>)> func)
{
	if(root)
	{
		if(root->Family() == T::Family()) func(std::static_pointer_cast<T>(root));
		for(auto child : root->children)
		{
			Traverse<T>(child, func);
		}
	}
}

std::string TreeToJSON(NodePtr root);