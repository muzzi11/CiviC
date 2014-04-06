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

	return sstream.str();
}

std::string AssemblyGenerator::Assign(std::shared_ptr<Assignment> root)
{
	std::stringstream sstream;

	TraverseBreadth<Literal>(root, [&](std::shared_ptr<Literal> literal, NodePtr parent)
	{
		if(literal->type == Type::Int) sstream << '\t' << VarInstr::LoadConstant(literal->intValue) << "\n";
		if(literal->type == Type::Float) sstream << '\t' << VarInstr::LoadConstant(literal->floatValue) << "\n";
		return true;
	});

	TraverseDepth<BinaryOp>(root, [&](std::shared_ptr<BinaryOp> op, NodePtr parent)
	{
		if(op->op == Operator::Multiply) sstream << '\t' << ArithInstr::Multiply(NodeTypeToInstrType(op->type)) << "\n";
		else if(op->op == Operator::Add) sstream << '\t' << ArithInstr::Add(NodeTypeToInstrType(op->type)) << "\n";
		return true;
	});

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