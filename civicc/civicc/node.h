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
		None,
		Bool,
		Int,
		Float,
		Void
	};

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
		bool exp;
		FunctionHeader header;

		std::string ToString() const override;
	};

	struct GlobalDef : public Node<GlobalDef>
	{
		bool exp;
		Variable var;

		std::string ToString() const override;
	};

	struct VarDec : public Node<VarDec>
	{
		Variable var;

		std::string ToString() const override;
	};

	struct ArrayExpr : public Node<ArrayExpr>
	{
	};

	struct Assignment : public Node<Assignment>
	{
		std::string name;

		std::string ToString() const override;
	};

	struct Return : public Node<Return>
	{
	};

	struct Call : public Node<Call>
	{
		std::string name;

		std::string ToString() const override;
	};

	/*struct Expr : public Node<Expr>
	{
	};*/

	struct BinaryOp : public Node<BinaryOp>
	{
		Operator op;

		BinaryOp(Operator op) : op(op) {}
	};

	struct UnaryOp : public Node<UnaryOp>
	{
		Operator op;

		UnaryOp(Operator op) : op(op) {}
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

		Identifier(const std::string& name) : name(name) {}
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
