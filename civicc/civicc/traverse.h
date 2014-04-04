#pragma once

#include <string>
#include <memory>
#include <functional>

#include "node.h"


// func(node, parent)
template<class T>
bool Traverse(Node::NodePtr root, std::function<bool(std::shared_ptr<T>, Node::NodePtr)> func, Node::NodePtr parent = nullptr)
{
	bool increment = true;

	if(root)
	{
		if(root->Family() == T::Family()) increment = func(std::static_pointer_cast<T>(root), parent);
		size_t i = 0;
		while(i < root->children.size())
		{
			if(Traverse<T>(root->children[i], func, root)) ++i;
		}
	}

	return increment;
}

// -1 for unlimited depth
template<class T>
void Traverse(Node::NodePtr root, std::function<void(std::shared_ptr<T>)> func, int depth)
{
	if(root)
	{
		if(root->Family() == T::Family()) func(std::static_pointer_cast<T>(root));
		if(depth < 0 || depth > 0)
		{
			for(auto child : root->children) Traverse<T>(child, func, depth - 1);
		}
		else
		{
			for(auto child : root->children)
			{
				if(child->Family() == T::Family()) func(std::static_pointer_cast<T>(child));
			}
		}
	}
}

template<class T>
int Count(Node::NodePtr root, int depth = -1)
{
	int count = 0;

	Traverse<T>(root, [&](std::shared_ptr<T>)
	{
		count++;
	}, depth);

	return count;
}

std::string TreeToJSON(Node::NodePtr root);