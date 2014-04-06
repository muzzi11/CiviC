#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <cinttypes>
#include <cassert>

#include "token.h"

namespace Nodes
{
	enum class Type
	{
		None,
		Bool,
		Int,
		Float,
		Void
	};
	std::string TypeToString(Type type);

	enum class Operator
	{
		Add,
		Subtract,
		Multiply,
		Divide,
		Modulo,
		Equal,
		NotEqual,
		Less,
		LessEqual,
		More,
		MoreEqual,
		And,
		Or,
		Negate,
		Not
	};
	
	class BaseNode;

	typedef std::shared_ptr<BaseNode> NodePtr;
	
	class BaseNode
	{
	public:
		int line, pos;
		std::vector<NodePtr> children;

		virtual std::string ToString() const;

		uint32_t Family() const;
		std::string FamilyName() const;

		template<class T>
		bool IsFamily() const
		{
			return Family() == T::Family();
		}

	protected:
		uint32_t family_ = ~0;
		static uint32_t familyCounter_;
		static std::unordered_map<uint32_t, std::string> familyNames_;
	};

	template<class T>
	struct Node : public BaseNode
	{
		Node()
		{
			family_ = Family();
			familyNames_[family_] = typeid(T).name();
		}

		static int32_t Family()
		{
			assert(BaseNode::familyCounter_ < 32 && "Number of node types exceeded maximum of 32");
			static uint32_t family = (1 << BaseNode::familyCounter_++);
			return family;
		}
	};

	template<class T>
	std::shared_ptr<T> StaticCast(NodePtr node)
	{
		return node->IsFamily<T>() ? std::static_pointer_cast<T>(node) : nullptr;
	}

	struct Variable
	{
		bool array = false;
		Type type;
		std::string name;
		int pos, line;
		
		std::string ToString() const;
	};

	struct Param
	{
		Type type;
		std::string name;
		std::vector<std::string> dim;
		int pos, line;

		std::string ToString() const;
	};

	struct FunctionHeader
	{
		Type returnType;
		std::string name;
		std::vector<Param> params;
		int pos, line;
		
		std::string ToString() const;
	};

	struct Root : public Node<Root>
	{
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
		bool exp;
		FunctionHeader header;

		std::string ToString() const override;
	};

	struct GlobalDef : public Node<GlobalDef>
	{
		bool exp;
		Variable var;

		bool HasAssignment() const;
		std::string ToString() const override;
	};

	struct VarDec : public Node<VarDec>
	{
		bool immutable = false;
		Variable var;

		bool HasAssignment() const;
		std::string ToString() const override;
	};

	struct ArrayExpr : public Node<ArrayExpr>
	{
	};

	struct Assignment : public Node<Assignment>
	{
		std::string name;
		NodePtr dec;
		Type type;

		Assignment(const std::string& name) : name(name) {}
		std::string ToString() const override;
	};

	struct Return : public Node<Return>
	{
		std::string functionName;
		Type type;
	};

	struct Call : public Node<Call>
	{
		std::string name;
		NodePtr decPtr;

		std::string ToString() const override;
	};

	struct BinaryOp : public Node<BinaryOp>
	{
		Operator op;
		Type type;

		BinaryOp(Operator op) : op(op) {}
		std::string ToString() const override;
	};

	struct UnaryOp : public Node<UnaryOp>
	{
		Operator op;
		Type type;

		UnaryOp(Operator op) : op(op) {}
		std::string ToString() const override;
	};

	struct Cast : public Node<Cast>
	{
		Type type;

		Cast(Type type) : type(type) {}
		std::string ToString() const override;
	};

	struct Literal : public Node<Literal>
	{
		Type type;
		union
		{
			bool boolValue;
			int intValue;
			float floatValue;
		};

		Literal(bool value) : type(Type::Bool), boolValue(value) {}
		Literal(int value) : type(Type::Int), intValue(value) {}
		Literal(float value) : type(Type::Float), floatValue(value) {}
		std::string ToString() const override;
	};

	struct Identifier : public Node<Identifier>
	{
		std::string name;
		NodePtr defOrDec;
		Identifier(const std::string& name) : name(name) {}
		std::string ToString() const override;
	};

	struct Ternary : public Node<Ternary>
	{
		std::string ToString() const override;
	};

	struct If : public Node<If>
	{
	};

	struct Else : public Node<Else>
	{
	};

	struct While : public Node<While>
	{
	};

	struct DoWhile : public Node<DoWhile>
	{
	};

	struct For : public Node<For>
	{
	};
}
