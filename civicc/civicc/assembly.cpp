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

	BuildTables(root);

	TraverseBreadth(root, [&](NodePtr node, NodePtr parent)
	{
		auto funDef = StaticCast<FunctionDef>(node);
		if(funDef) sstream << FunDef(funDef);
	});

	sstream << "\n; globals:\n";
	sstream << VarInstr::GetConstantTable();

	return sstream.str();
}

void AssemblyGenerator::BuildTables(Nodes::NodePtr root)
{
	int frame = 0, index = 0;
	NodePtr prev = root;
	
	NodePtr curDef = nullptr;
	TraverseBreadth(root, [&](NodePtr node, NodePtr parent)
	{
		auto funDef = StaticCast<FunctionDef>(node);
		if(funDef)
		{
			curDef = funDef;
			if(prev != parent)
			{
				frame++;
				prev = parent;
			}
			
			index = funDef->header.params.size();
			functionNestingTable[funDef] = frame;
		}

		if(node->IsFamily<GlobalDec>() || node->IsFamily<GlobalDef>()) globalIndexTable[node] = globalIndexTable.size();
		if(node->IsFamily<VarDec>()) localTable[node] = { frame, index++ };
		if(node->IsFamily<Assignment>()) assignFrameTable[node] = frame;
		if(node->IsFamily<Identifier>()) idFrameTable[node] = frame;
		if(node->IsFamily<Call>())
		{
			functionCallTable[node] = curDef && Count(curDef, node) ? frame + 1 : frame;
		}
	});
}

std::string AssemblyGenerator::FunDef(std::shared_ptr<FunctionDef> root)
{
	std::stringstream sstream;

	sstream << root->header.name << ":\n";
	int varCount = std::count_if(root->children.begin(), root->children.end(), [](NodePtr node)
	{
		return node->IsFamily<VarDec>();
	});
	sstream << '\t' << CntrlFlwInstr::EnterSub(varCount) << "\n";


	TraverseBreadth(root, [&](NodePtr node, NodePtr parent)
	{
		if(parent == root) sstream << Statements(node);
	});

	if(!root->children.empty())
	{
		auto ret = StaticCast<Return>(root->children.back());
		if(ret) sstream << Expression(ret) << '\t' << CntrlFlwInstr::Return(NodeTypeToInstrType(ret->type)) << '\n';
		else sstream << '\t' << CntrlFlwInstr::Return(Instr::Void) << '\n';
	}
	else sstream << '\t' << CntrlFlwInstr::Return(Instr::Void) << '\n';

	return sstream.str();
}

std::string AssemblyGenerator::Assign(std::shared_ptr<Assignment> root)
{
	std::stringstream sstream;
	Instr::Type type = NodeTypeToInstrType(root->type);

	sstream << Expression(root);

	if(root->dec->IsFamily<VarDec>())
	{
		int index = localTable[root->dec].index;
		int frame = assignFrameTable[root] - localTable[root->dec].frame;
		bool sameScope = frame == 0;

		if(sameScope) sstream << '\t' << VarInstr::StoreLocal(type, index) << '\n';
		else sstream << '\t' << VarInstr::StoreRelative(type, frame, index) << '\n';
	}
	else if(root->dec->IsFamily<FunctionDef>())
	{
		auto def = StaticCast<FunctionDef>(root->dec);
		auto params = def->header.params;
		int index = std::find_if(params.begin(), params.end(), [&](Param p) -> bool
		{
			return p.name == root->name;
		}) - params.begin();
		int frame = assignFrameTable[root] - functionNestingTable[root->dec];
		bool sameScope = frame == 0;

		if(sameScope) sstream << '\t' << VarInstr::StoreLocal(type, index) << '\n';
		else sstream << '\t' << VarInstr::StoreRelative(type, functionNestingTable[def], index) << '\n';
	}
	else
	{
		sstream << '\t' << VarInstr::StoreGlobal(type, globalIndexTable[root->dec]) << '\n';
	}

	return sstream.str();
}

std::string AssemblyGenerator::FunCall(std::shared_ptr<Call> call, bool expr)
{
	std::stringstream sstream;

	auto funDec = StaticCast<FunctionDec>(call->dec);
	if(funDec)
	{
		sstream << '\t' << CntrlFlwInstr::InitiateSub(CntrlFlwInstr::Scope::Global) << '\n';
		for(auto child : call->children) sstream << Expression(child);
		//!!!!!!!CntrlFlwInstr::JumpExtSub();
		if(!expr && funDec->header.returnType != Type::Void)
		{
			sstream << '\t' << StackInstr::Pop(NodeTypeToInstrType(funDec->header.returnType)) << '\n';
		}
	}
	else
	{
		auto funDef = StaticCast<FunctionDef>(call->dec);
		int funFrame = functionNestingTable[funDef];
		int callFrame = functionCallTable[call];

		sstream << '\t';
		if(funFrame == 0) sstream << CntrlFlwInstr::InitiateSub(CntrlFlwInstr::Scope::Global);
		else if((callFrame - funFrame) == 1 && (Count(funDef, call) > 0)) sstream << CntrlFlwInstr::InitiateSub(CntrlFlwInstr::Scope::Current);
		else if(funFrame == callFrame) sstream << CntrlFlwInstr::InitiateSub(CntrlFlwInstr::Scope::Local);
		else sstream << CntrlFlwInstr::InitiateSub(CntrlFlwInstr::Scope::Outer, callFrame - funFrame - 1);
		sstream << '\n';

		for(auto child : call->children) sstream << Expression(child);

		sstream << '\t' << CntrlFlwInstr::JumpSub(call->children.size(), call->name) << '\n';

		if(!expr && funDef->header.returnType != Type::Void)
		{
			sstream << '\t' << StackInstr::Pop(NodeTypeToInstrType(funDef->header.returnType)) << '\n';
		}
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
		{ Operator::Negate, &ArithInstr::Negate },
		{ Operator::Equal, &CompInstr::Equal },
		{ Operator::NotEqual, &CompInstr::NotEqual },
		{ Operator::More, &CompInstr::Greater },
		{ Operator::MoreEqual, &CompInstr::GreaterEqual },
		{ Operator::Less, &CompInstr::Less },
		{ Operator::LessEqual, &CompInstr::LessEqual },
	});

	NodePtr funcDef;
	TraverseBreadth(root, [&](NodePtr node, NodePtr parent)
	{
		if(parent && parent->IsFamily<Call>()) return;

		if(node->IsFamily<FunctionDef>()) funcDef = node;

		auto literal = StaticCast<Literal>(node);
		if(literal)
		{
			if(!parent || parent && !parent->IsFamily<Ternary>())
			{
				if(literal->type == Type::Int) sstream << '\t' << VarInstr::LoadConstant(literal->intValue) << '\n';
				else if(literal->type == Type::Float) sstream << '\t' << VarInstr::LoadConstant(literal->floatValue) << '\n';
				else sstream << '\t' << VarInstr::LoadConstant(literal->boolValue) << '\n';
			}
		}

		auto unOp = StaticCast<UnaryOp>(node);
		if(unOp) sstream << '\t' << arithOpMap[unOp->op](NodeTypeToInstrType(unOp->type)) << '\n';

		auto binOp = StaticCast<BinaryOp>(node);
		if(binOp) sstream << '\t' << arithOpMap[binOp->op](NodeTypeToInstrType(binOp->type)) << '\n';

		auto call = StaticCast<Call>(node);
		if(call) sstream << FunCall(call, true);

		auto cast = StaticCast<Cast>(node);
		if(cast)
		{
			if(cast->type == Type::Int) sstream << '\t' << CastInstr::Float2Int() << '\n';
			else sstream << '\t' << CastInstr::Int2Float() << '\n';
		}

		auto ternary = StaticCast<Ternary>(node);
		if(ternary)
		{
			std::string branch, end;
			std::stringstream label;
			label << labelCounter++;
			branch = label.str() + "_false_expr";
			end = label.str() + "_end";

			sstream << Expression(ternary->children[0]);
			sstream << '\t' << CntrlFlwInstr::Branch(false, branch) << '\n';
			sstream << Expression(ternary->children[1]);
			sstream << '\t' << CntrlFlwInstr::Jump(end) << '\n';
			sstream << branch << ":\n";
			sstream << Expression(ternary->children[2]);
			sstream << end << ":\n";
		}

		auto id = StaticCast<Identifier>(node);
		if(id)
		{
			Instr::Type type = NodeTypeToInstrType(id->type);

			if(id->dec->IsFamily<VarDec>())
			{
				int index = localTable[id->dec].index;
				int frame = idFrameTable[id] - localTable[id->dec].frame;
				bool sameScope = frame == 0;

				if(sameScope) sstream << '\t' << VarInstr::LoadLocal(type, index) << '\n';
				else sstream << '\t' << VarInstr::LoadRelative(type, frame, index) << '\n';
			}
			else if(id->dec->IsFamily<FunctionDef>())
			{
				auto def = StaticCast<FunctionDef>(id->dec);
				auto params = def->header.params;
				int index = std::find_if(params.begin(), params.end(), [&](Param p) -> bool
				{
					return p.name == id->name;
				}) - params.begin();
				int frame = idFrameTable[root] - functionNestingTable[def];
				bool sameScope = frame == 0;

				if(sameScope) sstream << '\t' << VarInstr::LoadLocal(type, index) << '\n';
				else sstream << '\t' << VarInstr::LoadRelative(type, functionNestingTable[def], index) << '\n';
			}
			else
			{
				sstream << '\t' << VarInstr::LoadGlobal(type, globalIndexTable[id->dec]) << '\n';
			}
		}
	});

	return sstream.str();
}

std::string AssemblyGenerator::Statements(NodePtr root)
{
	std::stringstream sstream;

	auto ifStatement = StaticCast<If>(root);
	if(ifStatement) 
		sstream << IfElse(ifStatement);

	sstream << DoWhileLoop(root);

	auto assign = StaticCast<Assignment>(root);
	if(assign) sstream << Assign(assign);

	auto call = StaticCast<Call>(root);
	if(call) sstream << FunCall(call, false);

	return sstream.str();
}

std::string AssemblyGenerator::IfElse(NodePtr root)
{
	std::stringstream sstream;

	auto ifStatement = StaticCast<If>(root);
	if(ifStatement)
	{
		std::string branch, end;
		std::stringstream label;
		label << labelCounter++;
		branch = label.str() + "_else";
		end = label.str() + "_end";

		if(ifStatement->children.back()->IsFamily<Else>())
		{
			auto elseStatement = StaticCast<Else>(ifStatement->children.back());

			sstream << Expression(ifStatement->children[0]);
			sstream << '\t' << CntrlFlwInstr::Branch(false, branch) << '\n';
			for(size_t i = 1; i < ifStatement->children.size() - 1; ++i) sstream << Statements(ifStatement->children[i]);
			sstream << '\t' << CntrlFlwInstr::Jump(end) <<'\n';
			sstream << branch << ":\n";
			for(auto child : elseStatement->children) sstream << Statements(child);
			sstream << end << ":\n";
		}
		else
		{
			sstream << Expression(ifStatement->children[0]);
			sstream << '\t' << CntrlFlwInstr::Branch(false, branch) << '\n';
			for(size_t i = 1; i < ifStatement->children.size(); ++i) sstream << Statements(ifStatement->children[i]);
			sstream << branch << ":\n";
		}
	}

	return sstream.str();
}

std::string AssemblyGenerator::DoWhileLoop(NodePtr root)
{
	std::stringstream sstream;

	auto doWhile = StaticCast<DoWhile>(root);
	if(doWhile)
	{
		std::stringstream label;
		label << labelCounter++ << "_do_while";

		sstream << label.str() << ":\n";
		for(size_t i = 0; i < doWhile->children.size() - 1; ++i) sstream << Statements(doWhile->children[i]);
		sstream << Expression(doWhile->children.back());
		sstream << CntrlFlwInstr::Branch(true, label.str()) << '\n';
	}

	return sstream.str();
}