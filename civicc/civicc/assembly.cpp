#include <sstream>
#include <unordered_map>
#include <algorithm>

#include "instruction.h"
#include "assembly.h"
#include "traverse.h"

using namespace Nodes;


Instr::Type NodeTypeToInstrType(Type type)
{
	static const std::unordered_map<Type, Instr::Type> map(
	{
		{ Type::Bool, Instr::Type::Bool },
		{ Type::Int, Instr::Type::Int },
		{ Type::Float, Instr::Type::Float },
		{ Type::Void, Instr::Type::Void },
	});

	return map.at(type);
}

std::string AssemblyGenerator::Generate(NodePtr root)
{
	std::stringstream sstream;

	BuildVariableTables(root);

	TraverseBreadth(root, [&](NodePtr node, NodePtr parent)
	{
		if(node->IsFamily<FunctionDef>())
		{
			sstream << FunDef(std::static_pointer_cast<FunctionDef>(node));
		}

		return true;
	});

	sstream << "\n; globals:\n";
	sstream << VarInstr::GetConstantTable();

	return sstream.str();
}

void AssemblyGenerator::BuildVariableTables(Nodes::NodePtr root)
{
	int frame = 0, index = 0;
	NodePtr cur = nullptr;

	TraverseBreadth(root, [&](NodePtr node, NodePtr parent)
	{
		if(node->IsFamily<FunctionDef>() && cur != parent)
		{
			frame++;
			index = std::static_pointer_cast<FunctionDef>(node)->header.params.size();
		}

		if(node->IsFamily<GlobalDec>() || node->IsFamily<GlobalDef>()) globalIndexTable[node] = globalIndexTable.size();
		if(node->IsFamily<VarDec>()) localTable[node] = { frame, index };
		if(node->IsFamily<Assignment>()) assignFrameTable[node] = frame;

		return true;
	});
}

std::string AssemblyGenerator::FunDef(std::shared_ptr<FunctionDef> root)
{
	std::stringstream sstream;

	sstream << root->header.name << ":\n";
	sstream << '\t' << CntrlFlwInstr::EnterSub(Count<VarDec>(root)) << "\n";

	TraverseBreadth<Assignment>(root, [&](std::shared_ptr<Assignment> assign, NodePtr parent)
	{
		sstream << Assign(assign);
		return false;
	});

	if(root->children.back()->IsFamily<Return>())
	{
		auto ret = std::static_pointer_cast<Return>(root->children.back());

	}

	return sstream.str();
}

std::string AssemblyGenerator::Assign(std::shared_ptr<Assignment> root)
{
	std::stringstream sstream;

	sstream << Expression(root);

	if(root->dec->IsFamily<GlobalDec>() || root->dec->IsFamily<GlobalDef>())
	{
		sstream << '\t' << VarInstr::StoreGlobal(NodeTypeToInstrType(root->type), globalIndexTable[root->dec]);
	}
	else if(root->dec->IsFamily<VarDec>())
	{
		sstream << '\t' << VarInstr::StoreLocal(NodeTypeToInstrType(root->type), localTable[root->dec].index);
	}
	else
	{
		int index = localTable[root->dec].index;
		int frame = assignFrameTable[root] - localTable[root->dec].frame;
		sstream << '\t' << VarInstr::StoreRelative(NodeTypeToInstrType(root->type), index, frame);
	}

	return sstream.str();
}

std::string AssemblyGenerator::Expression(NodePtr root)
{
	std::stringstream sstream;
	std::unordered_map<Operator, std::function<std::string(Instr::Type)>> arithOpMap(
	{
		{ Operator::Add, &ArithInstr::Add },
		{ Operator::Subtract, &ArithInstr::Sub },
		{ Operator::Multiply, &ArithInstr::Multiply },
		{ Operator::Divide, &ArithInstr::Divide },
		{ Operator::Modulo, &ArithInstr::Modulo },
		{ Operator::Not, &ArithInstr::Not },
		{ Operator::Negate, &ArithInstr::Negate }
	});

	TraverseBreadth<Literal>(root, [&](std::shared_ptr<Literal> literal, NodePtr parent)
	{
		if(literal->type == Type::Int) sstream << '\t' << VarInstr::LoadConstant(literal->intValue) << '\n';
		else if(literal->type == Type::Float) sstream << '\t' << VarInstr::LoadConstant(literal->floatValue) << '\n';
		else sstream << '\t' << VarInstr::LoadConstant(literal->boolValue) << '\n';
		return true;
	});

	TraverseDepth(root, [&](NodePtr node, NodePtr parent)
	{
		auto unOp = StaticCast<UnaryOp>(node);
		if(unOp) sstream << '\t' << arithOpMap[unOp->op](NodeTypeToInstrType(unOp->type)) << '\n';

		auto binOp = StaticCast<BinaryOp>(node);
		if(binOp) sstream << '\t' << arithOpMap[binOp->op](NodeTypeToInstrType(binOp->type)) << '\n';

		auto call = StaticCast<Call>(node);
		if(call)
		{

		}

		return true;
	});

	return sstream.str();
}