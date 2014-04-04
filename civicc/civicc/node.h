#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <typeindex>

#include "token.h"

namespace Node
{
	enum class Type
	{
		Bool,
		Int,
		Float,
		Void
	};

	class BaseNode;

	typedef std::shared_ptr<BaseNode> NodePtr;

	class BaseNode
	{
	public:
		std::vector<NodePtr> children;

		virtual std::string ToString() const;

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
		Node()
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

	struct Variable
	{
		Type type;
		std::string name;
		int dim;

		std::string ToString() const;
	};

	struct Param
	{
		Type type;
		std::string name;
		std::vector<std::string> dim;

		std::string ToString() const;
	};

	struct FunctionHeader
	{
		Type returnType;
		std::string name;
		std::vector<Param> params;

		std::string ToString() const;
	};

	struct FunctionDec : public Node<FunctionDec>
	{
		FunctionHeader header;

		std::string ToString() const override;
	};

	struct GlobalDec : public Node<GlobalDec>
	{
		Param param;

		std::string ToString() const override;
	};

	struct FunctionDef : public Node<FunctionDef>
	{
		bool export;
		FunctionHeader header;

		std::string ToString() const override;
	};

	struct GlobalDef : public Node<GlobalDef>
	{
		bool export;
		Variable var;

		std::string ToString() const override;
	};

	struct VarDec : public Node<VarDec>
	{
		Variable var;

		std::string ToString() const override;
	};

	struct Assignment : public Node<Assignment>
	{
		std::string name;
		int dim;

		std::string ToString() const override;
	};

	struct Return : public Node<Return>
	{
	};

	struct Expr : public Node<Expr>
	{
	};

	struct BinaryOpNode : public Node<BinaryOpNode>
	{
		BinaryOpNode(const Token& token) {}
	};

	struct UnaryOpNode : public Node<UnaryOpNode>
	{
		UnaryOpNode(const Token& token) {}
	};

	struct IdentifierNode : public Node<IdentifierNode>
	{
		IdentifierNode(const Token& token) {}
	};

	struct LiteralNode : public Node<LiteralNode>
	{
		LiteralNode(const Token& token) {}
	};

	struct CallNode : public Node<CallNode>
	{
		CallNode(const Token& token) {}
	};
}