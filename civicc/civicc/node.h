#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <unordered_map>

#include "token.h"

class BaseNode;

typedef std::shared_ptr<BaseNode> NodePtr;

class BaseNode
{
public:	
	std::vector<NodePtr> children;
	Token token;

	BaseNode(const Token& token) : token(token) {}

	int Family() const;
	std::string FamilyName() const;

protected:
	int family_;
	static int familyCounter_;
	static std::unordered_map<int, std::string> familyNames_;
};

template<class T>
struct Node : public BaseNode
{
	Node(const Token& token) : BaseNode(token)
	{
		family_ = Family();
		familyNames_[family_] = typeid(T).name();
	}

	static int Family()
	{
		static int family = BaseNode::familyCounter_++;
		return family;
	}
};

struct BinaryOpNode : public Node<BinaryOpNode>
{
	BinaryOpNode(const Token& token) : Node<BinaryOpNode>(token) {}
};

struct UnaryOpNode : public Node<UnaryOpNode>
{
	UnaryOpNode(const Token& token) : Node<UnaryOpNode>(token) {}
};

struct IdentifierNode : public Node<IdentifierNode>
{
	IdentifierNode(const Token& token) : Node<IdentifierNode>(token) {}
};

struct LiteralNode : public Node<LiteralNode>
{
	LiteralNode(const Token& token) : Node<LiteralNode>(token) {}
};

struct CallNode : public Node<CallNode>
{
	CallNode(const Token& token) : Node<CallNode>(token) {}
};

void Traverse(NodePtr root, std::function<void(NodePtr)> func);

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