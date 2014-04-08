#pragma once

#include <string>
#include <memory>
#include <functional>

#include "node.h"


// bool func(node, parent) - return false to terminate traversel
template<class T>
void TraverseBreadth(Nodes::NodePtr root, std::function<void(std::shared_ptr<T>, Nodes::NodePtr)> func, Nodes::NodePtr parent = nullptr)
{
	if(root)
	{
		if(root->Family() & T::Family() && parent == nullptr) func(std::static_pointer_cast<T>(root), parent);
		size_t i = 0;
		while(i < root->children.size())
		{
			auto child = root->children[i++];
			if(child->Family() == T::Family()) func(std::static_pointer_cast<T>(child), root);
		}
		i = 0;
		while(i < root->children.size()) TraverseBreadth(root->children[i++], func, root);
	}
}

template<class T>
void TraverseDepth(Nodes::NodePtr root, std::function<void(std::shared_ptr<T>, Nodes::NodePtr)> func, Nodes::NodePtr parent = nullptr)
{
	if(root)
	{
		size_t i = 0;
		while(i < root->children.size()) TraverseDepth(root->children[i++], func, root);
		if(root->Family() & T::Family())
		{
			func(std::static_pointer_cast<T>(root), parent);
		}
	}
}

void TraverseBreadth(Nodes::NodePtr root, std::function<void(Nodes::NodePtr, Nodes::NodePtr)> func, Nodes::NodePtr parent = nullptr);
void TraverseDepth(Nodes::NodePtr root, std::function<void(Nodes::NodePtr, Nodes::NodePtr)> func, Nodes::NodePtr parent = nullptr);

template<class T>
void Replace(Nodes::NodePtr& root, std::function<Nodes::NodePtr(std::shared_ptr<T>)> func)
{
	if(root)
	{
		size_t i = 0;
		while(i < root->children.size()) Replace(root->children[i++], func);
		if(root->Family() == T::Family()) root = func(std::static_pointer_cast<T>(root));
	}
}
template<class T>
void ReplaceBreadth(Nodes::NodePtr& root, std::function<Nodes::NodePtr(std::shared_ptr<T>, std::shared_ptr<T> parent)> func)
{
	if (root)
	{
		size_t i = 0;
		while (i < root->children.size())
		{
			auto child = root->children[i++];
			if (child->Family() == T::Family()) child = func(std::static_pointer_cast<T>(child), std::static_pointer_cast<T>(root));
		}
		i = 0;
		while (i < root->children.size()) ReplaceBreadth(root->children[i++], func);
	}
}

// -1 for unlimited depth
/*
template<class T>
void Traverse(Nodes::NodePtr root, std::function<void(std::shared_ptr<T>)> func, int depth)
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
}*/

template<class T>
int Count(Nodes::NodePtr root)
{
	int count = 0;

	TraverseBreadth<T>(root, [&](std::shared_ptr<T>, Nodes::NodePtr)
	{
		count++;
	});

	return count;
}

int Count(Nodes::NodePtr root, Nodes::NodePtr val);

std::string TreeToJSON(Nodes::NodePtr root);