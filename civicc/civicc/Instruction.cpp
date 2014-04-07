#include <sstream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <string>
#include <assert.h>

#include "instruction.h"

const std::string Instr::TypeNames::Float = "float";
const std::string Instr::TypeNames::Int = "int";

const std::string Instr::ParseInstr(const std::string instr, const int arg)
{
	std::stringstream sstream;
	sstream << instr << " " << arg;
	return sstream.str();
}

const std::string Instr::ParseInstr(const std::string instr, const std::string arg)
{
	std::stringstream sstream;
	sstream << instr << " " << arg;
	return sstream.str();
}

const std::string Instr::ParseInstr(const std::string instr, const std::string arg1, const int arg2)
{
	std::stringstream sstream;
	sstream << instr << " " << arg1 << " " << arg2;
	return sstream.str();
}

const std::string Instr::ParseInstr(const std::string instr, const int arg1, const std::string arg2)
{
	std::stringstream sstream;
	sstream << instr << " " << arg1 << " " << arg2;
	return sstream.str();
}

const std::string Instr::ParseInstr(const std::string instr, const int arg1, const int arg2)
{
	std::stringstream sstream;
	sstream << instr << " " << arg1 << " " << arg2;
	return sstream.str();
}

const std::string ArithInstr::iAdd = "iadd";
const std::string ArithInstr::fAdd = "fadd";

const std::string ArithInstr::iSub = "isub";
const std::string ArithInstr::fSub = "fsub";

const std::string ArithInstr::iMul = "imul";
const std::string ArithInstr::fMul = "fmul";

const std::string ArithInstr::iDiv = "idiv";
const std::string ArithInstr::fDiv = "fdiv";

const std::string ArithInstr::iRem = "irem";

const std::string ArithInstr::iNeg = "ineg";
const std::string ArithInstr::fNeg = "fneg";
const std::string ArithInstr::bNot = "bnot";

const std::string ArithInstr::iInc = "iinc";
const std::string ArithInstr::iInc_1 = "iinc_1";
const std::string ArithInstr::iDec = "idec";
const std::string ArithInstr::iDec_1 = "idec_1";

const std::string ArithInstr::Add(Instr::Type type)
{
	assert(type == Instr::Int || type == Instr::Float);
	return (type == Instr::Int) ? iAdd : fAdd;
}

const std::string ArithInstr::Sub(Instr::Type type)
{
	assert(type == Instr::Int || type == Instr::Float);
	return (type == Instr::Int) ? iSub : fSub;
}

const std::string ArithInstr::Multiply(Instr::Type type)
{
	assert(type == Instr::Int || type == Instr::Float);
	return (type == Instr::Int) ? iMul : fMul;
}

const std::string ArithInstr::Divide(Instr::Type type)
{
	assert(type == Instr::Int || type == Instr::Float);
	return (type == Instr::Int) ? iDiv : fDiv;
}

const std::string ArithInstr::Modulo(Instr::Type type)
{
	assert(type == Instr::Int);
	return iRem;
}

const std::string ArithInstr::Negate(Instr::Type type)
{
	assert(type == Instr::Int || type == Instr::Float);
	return (type == Instr::Int) ? iNeg : fNeg;
}

const std::string ArithInstr::Not(Instr::Type type)
{
	assert(type == Instr::Bool);
	return bNot;
}

const std::string ArithInstr::Increment(const std::string local, const int constant)
{
	if (constant == 1) return Instr::ParseInstr(iInc_1, local);
	return Instr::ParseInstr(iInc, local, constant);
}

const std::string ArithInstr::Decrement(const std::string local, const int constant)
{
	if (constant == 1) return Instr::ParseInstr(iDec_1, local);
	return Instr::ParseInstr(iDec, local, constant);
}

const std::string CompInstr::iNotEqual = "ine";
const std::string CompInstr::iEqual = "ieq";
const std::string CompInstr::iLess = "ilt";
const std::string CompInstr::iLessEqual = "ile";
const std::string CompInstr::iGreater = "igt";
const std::string CompInstr::iGreaterEqual = "ige";

const std::string CompInstr::fNotEqual = "fne";
const std::string CompInstr::fEqual = "feq";
const std::string CompInstr::fLess = "flt";
const std::string CompInstr::fLessEqual = "fle";
const std::string CompInstr::fGreater = "igt";
const std::string CompInstr::fGreaterEqual = "ige";

const std::string CompInstr::bNotEqual = "bne";
const std::string CompInstr::bEqual = "beq";

const std::string CompInstr::NotEqual(Instr::Type type)
{
	assert(type == Instr::Int || type == Instr::Float || type == Instr::Bool);
	if (type == Instr::Int) return iNotEqual;
	if (type == Instr::Float) return fNotEqual;
	else return bNotEqual;
}

const std::string CompInstr::Equal(Instr::Type type)
{
	assert(type == Instr::Int || type == Instr::Float || type == Instr::Bool);
	if (type == Instr::Int) return iEqual;
	if (type == Instr::Float) return fEqual;
	else return bEqual;
}

const std::string CompInstr::Less(Instr::Type type)
{
	assert(type == Instr::Int || type == Instr::Float);
	return (type == Instr::Int) ? iLess : fLess;
}

const std::string CompInstr::LessEqual(Instr::Type type)
{
	assert(type == Instr::Int || type == Instr::Float);
	return (type == Instr::Int) ? iLessEqual : fLessEqual;
}

const std::string CompInstr::Greater(Instr::Type type)
{
	assert(type == Instr::Int || type == Instr::Float);
	return (type == Instr::Int) ? iGreater : fGreater;
}

const std::string CompInstr::GreaterEqual(Instr::Type type)
{
	assert(type == Instr::Int || type == Instr::Float);
	return (type == Instr::Int) ? iGreaterEqual : fGreaterEqual;
}

const std::string CntrlFlwInstr::isr = "isr";
const std::string CntrlFlwInstr::isrn = "isrn";
const std::string CntrlFlwInstr::isrl = "isrl";
const std::string CntrlFlwInstr::isrg = "isrg";

const std::string CntrlFlwInstr::jsr = "jsr";
const std::string CntrlFlwInstr::jsre = "jsre";

const std::string CntrlFlwInstr::esr = "esr";

const std::string CntrlFlwInstr::iReturn = "ireturn";
const std::string CntrlFlwInstr::fReturn = "freturn";
const std::string CntrlFlwInstr::bReturn = "breturn";
const std::string CntrlFlwInstr::vReturn = "return";

const std::string CntrlFlwInstr::jump = "jump";
const std::string CntrlFlwInstr::branch_t = "branch_t";
const std::string CntrlFlwInstr::branch_f = "branch_f";

const std::string CntrlFlwInstr::InitiateSub(Scope scope, const int nestingLevels)
{
	if (scope == Current) return isr;
	if (scope == Outer) return Instr::ParseInstr(isrn, nestingLevels);
	if (scope == Local) return isrl;
	else return isrg;
}

const std::string CntrlFlwInstr::JumpSub(const int arguments, const int offset)
{
	return Instr::ParseInstr(jsr, arguments, offset);
}

const std::string CntrlFlwInstr::JumpSub(const int arguments, const std::string label)
{
	return Instr::ParseInstr(jsr, arguments, label);
}

const std::string CntrlFlwInstr::JumpExtSub(const int index)
{
	return Instr::ParseInstr(jsr, index);
}

const std::string CntrlFlwInstr::EnterSub(const int elements)
{
	return Instr::ParseInstr(esr, elements);
}

const std::string CntrlFlwInstr::Return(Instr::Type type)
{
	assert(type == Instr::Int || type == Instr::Float || type == Instr::Bool || type == Instr::Void);
	if (type == Instr::Int) return iReturn;
	if (type == Instr::Float) return fReturn;
	if (type == Instr::Bool) return bReturn;
	else return vReturn;
}

const std::string CntrlFlwInstr::Jump(const int offset)
{
	return Instr::ParseInstr(jump, offset);
}

const std::string CntrlFlwInstr::Jump(const std::string label)
{
	return Instr::ParseInstr(jump, label);
}

const std::string CntrlFlwInstr::Branch(bool condition, const int offset)
{
	return (condition) ? Instr::ParseInstr(branch_t, offset) : Instr::ParseInstr(branch_f, offset);
}

const std::string CntrlFlwInstr::Branch(bool condition, const std::string label)
{
	return (condition) ? Instr::ParseInstr(branch_t, label) : Instr::ParseInstr(branch_f, label);
}

std::vector<std::tuple<std::string, std::string>> VarInstr::constants;

const std::string VarInstr::iLoad = "iload";
const std::string VarInstr::fLoad = "fload";
const std::string VarInstr::bLoad = "bload";
const std::string VarInstr::aLoad = "aload";

const std::string VarInstr::iLoadN = "iloadn";
const std::string VarInstr::fLoadN = "floadn";
const std::string VarInstr::bLoadN = "bloadn";
const std::string VarInstr::aLoadN = "aloadn";

const std::string VarInstr::iLoadG = "iloadg";
const std::string VarInstr::fLoadG = "floadg";
const std::string VarInstr::bLoadG = "bloadg";
const std::string VarInstr::aLoadG = "aloadg";

const std::string VarInstr::iLoadC = "iloadc";
const std::string VarInstr::fLoadC = "floadc";
const std::string VarInstr::bLoadCF = "bloadc_f";
const std::string VarInstr::bLoadCT = "bloadc_t";

const std::string VarInstr::iStore = "istore";
const std::string VarInstr::fStore = "fstore";
const std::string VarInstr::bStore = "bstore";
const std::string VarInstr::aStore = "astore";

const std::string VarInstr::iStoreN = "istoren";
const std::string VarInstr::fStoreN = "fstoren";
const std::string VarInstr::bStoreN = "bstoren";

const std::string VarInstr::iStoreG = "istoreg";
const std::string VarInstr::fStoreG = "fstoreg";
const std::string VarInstr::bStoreG = "bstoreg";
const std::string VarInstr::aStoreG = "astoreg";

const std::string VarInstr::LoadLocal(Instr::Type type, const int index)
{
	assert(type == Instr::Int || type == Instr::Float || type == Instr::Bool || type == Instr::Array);
	std::string instr;
	if (type == Instr::Int) instr = iLoad;
	else if (type == Instr::Float) instr = fLoad;
	else if (type == Instr::Bool) instr = bLoad;
	else instr = aLoad;

	std::stringstream sstream;
	if (index >= 0 && index <= 3) sstream << instr << "_" << index;
	else sstream << instr << " " << index;
	return sstream.str();
}

const std::string VarInstr::StoreLocal(Instr::Type type, const int index)
{
	assert(type == Instr::Int || type == Instr::Float || type == Instr::Bool || type == Instr::Array);
	std::string instr;
	if (type == Instr::Int) instr = iStore;
	else if (type == Instr::Float) instr = fStore;
	else if (type == Instr::Bool) instr = bStore;
	else instr = aStore;
	
	return Instr::ParseInstr(instr, index);
}

const std::string VarInstr::LoadRelative(Instr::Type type, const int levels, const int index)
{
	assert(type == Instr::Int || type == Instr::Float || type == Instr::Bool || type == Instr::Array);
	std::string instr;
	if (type == Instr::Int) instr = iLoadN;
	else if (type == Instr::Float) instr = fLoadN;
	else if (type == Instr::Bool) instr = bLoadN;
	else instr = aLoadN;

	return Instr::ParseInstr(instr, levels, index);
}

const std::string VarInstr::StoreRelative(Instr::Type type, const int levels, const int index)
{
	assert(type == Instr::Int || type == Instr::Float || type == Instr::Bool);
	std::string instr;
	if (type == Instr::Int) instr = iStoreN;
	else if (type == Instr::Float) instr = fStoreN;
	else instr = bStoreN;

	return Instr::ParseInstr(instr, levels, index);
}

const std::string VarInstr::LoadGlobal(Instr::Type type, const int index)
{
	assert(type == Instr::Int || type == Instr::Float || type == Instr::Bool || type == Instr::Array);
	std::string instr;
	if (type == Instr::Int) instr = iLoadG;
	else if (type == Instr::Float) instr = fLoadG;
	else if (type == Instr::Bool) instr = bLoadG;
	else instr = aLoadG;

	return Instr::ParseInstr(instr, index);
}

const std::string VarInstr::StoreGlobal(Instr::Type type, const int index)
{
	assert(type == Instr::Int || type == Instr::Float || type == Instr::Bool || type == Instr::Array);
	std::string instr;
	if (type == Instr::Int) instr = iStoreG;
	else if (type == Instr::Float) instr = fStoreG;
	else if (type == Instr::Bool) instr = bStoreG;
	else instr = aStoreG;

	return Instr::ParseInstr(instr, index);
}

const std::string VarInstr::LoadConstant(const int value)
{
	std::stringstream sstream;
/*	if (value >= 0 && value <= 1)
	{
		sstream << iLoadC << "_" << value;
		return sstream.str();
	}*/
	if (value == -1)
	{
		sstream << iLoadC << "_m1";
		return sstream.str();
	}

	sstream << value;
	return LoadConstant(iLoadC, sstream.str(), Instr::TypeNames::Int);
}

const std::string VarInstr::LoadConstant(const float value)
{
	std::stringstream sstream;
	/*if (value == 0.0f)
	{
		sstream << fLoadC << "_0";
		return sstream.str();
	}
	if (value == 1.0f)
	{
		sstream << fLoadC << "_1";
		return sstream.str();
	}*/
	
	sstream << value;
	return LoadConstant(fLoadC, sstream.str(), Instr::TypeNames::Float);
}

const std::string VarInstr::LoadConstant(const bool value)
{
	return (value) ? bLoadCT : bLoadCF;
}

const std::string VarInstr::LoadConstant(const std::string instr, const std::string value, const std::string type)
{
	int index = -1;
	std::tuple<std::string, std::string> element(type, value);
	
	for (auto it = constants.begin(); it < constants.end(); ++it)
		if (*it == element) index = it - constants.begin();

	if (index == -1)
	{
		constants.push_back(element);
		index = constants.size() - 1;
	}
	
	std::stringstream sstream;	
	sstream << instr << " " << index;
	return sstream.str();
}

const std::string VarInstr::GetConstantTable()
{
	std::stringstream sstream;
	for (auto it = constants.begin(); it < constants.end(); ++it)
		sstream << ".const " << std::get<0>(*it) << " " << std::get<1>(*it) << std::endl;
	return sstream.str();
}

const std::string ArrayInstr::iNew = "inewa";
const std::string ArrayInstr::fNew = "fnewa";
const std::string ArrayInstr::bNew = "bnewa";

const std::string ArrayInstr::size = "asize";

const std::string ArrayInstr::iRead = "iloada";
const std::string ArrayInstr::fRead = "floada";
const std::string ArrayInstr::bRead = "bloada";

const std::string ArrayInstr::iStore = "istorea";
const std::string ArrayInstr::fStore = "fstorea";
const std::string ArrayInstr::bStore = "bstorea";

const std::string ArrayInstr::Size(const int dimension)
{
	return Instr::ParseInstr(size, dimension);
}

const std::string ArrayInstr::New(const Instr::Type type, const int dimensions)
{
	assert(type == Instr::Int || type == Instr::Float || type == Instr::Bool);
	if (type == Instr::Int) return Instr::ParseInstr(iNew, dimensions);
	if (type == Instr::Float) return Instr::ParseInstr(fNew, dimensions);
	return Instr::ParseInstr(bNew, dimensions);
}

const std::string ArrayInstr::Read(const Instr::Type type)
{
	assert(type == Instr::Int || type == Instr::Float || type == Instr::Bool);
	if (type == Instr::Int) return iRead;
	if (type == Instr::Float) return iRead;
	return bRead;
}

const std::string ArrayInstr::Store(const Instr::Type type)
{
	assert(type == Instr::Int || type == Instr::Float || type == Instr::Bool);
	if (type == Instr::Int) return iStore;
	if (type == Instr::Float) return fStore;
	return bStore;
}

const std::string CastInstr::int2Float = "i2f";
const std::string CastInstr::float2Int = "f2i";

const std::string CastInstr::Int2Float()
{
	return int2Float;
}

const std::string CastInstr::Float2Int()
{
	return float2Int;
}

const std::string StackInstr::iPop = "ipop";
const std::string StackInstr::fPop = "fpop";
const std::string StackInstr::bPop = "bpop";

const std::string StackInstr::Pop(Instr::Type type)
{
	assert(type == Instr::Int || type == Instr::Float || type == Instr::Bool);
	if (type == Instr::Int) return iPop;
	if (type == Instr::Float) return fPop;
	return bPop;
}