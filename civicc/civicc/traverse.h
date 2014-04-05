#pragma once

#include <string>
#include <memory>
#include <functional>

#include "node.h"


// bool func(node, parent) - return false to terminate traversel
template<class T>
void TraverseBreadth(Node::NodePtr root, std::function<bool(std::shared_ptr<T>, Node::NodePtr)> func, Node::NodePtr parent = nullptr)
{
	if(root)
	{
		if(root->Family() == T::Family() && parent == nullptr)
		{
			if(!func(std::static_pointer_cast<T>(root), parent)) return;
		}
		size_t i = 0;
		while(i < root->children.size())
		{
			auto child = root->children[i++];

			if(child->Family() == T::Family())
			{
				if(!func(std::static_pointer_cast<T>(child), root)) return;
			}
		}
		i = 0;
		while(i < root->children.size()) TraverseBreadth(root->children[i++], func, root);
	}
}

template<class T>
void TraverseDepth(Node::NodePtr root, std::function<bool(std::shared_ptr<T>, Node::NodePtr)> func, Node::NodePtr parent = nullptr)
{
	if(root)
	{
		size_t i = 0;
		while(i < root->children.size()) TraverseDepth(root->children[i++], func, root);
		if(root->Family() == T::Family())
		{
			if(!func(std::static_pointer_cast<T>(root), parent)) return;
		}
	}
}

template<class T>
void Replace(Node::NodePtr& root, std::function<Node::NodePtr(std::shared_ptr<T>)> func)
{
	if(root)
	{
		size_t i = 0;
		while(i < root->children.size()) Replace(root->children[i++], func);
		if(root->Family() == T::Family()) root = func(std::static_pointer_cast<T>(root));
	}
}

// -1 for unlimited depth
/*
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
}*/

std::string TreeToJSON(Node::NodePtr root);