#pragma once

#include <vector>
#include <tuple>
#include <string>

class Instr
{
public:
	enum Type
	{
		Int,
		Float,
		Bool,
		Void,
		Array
	};
	static class TypeNames
	{
	public:
		static const std::string Int;
		static const std::string Float;
	};

	static const std::string ParseInstr(const std::string instr, const int arg);
	static const std::string ParseInstr(const std::string instr, const std::string arg);
	static const std::string ParseInstr(const std::string instr, const std::string arg1, const int arg2);
	static const std::string ParseInstr(const std::string instr, const int arg1, const std::string arg2);
	static const std::string ParseInstr(const std::string instr, const int arg1, const int arg2);
};

class ArithInstr
{
public:	
	static const std::string Add(Instr::Type type);
	static const std::string Sub(Instr::Type type);
	static const std::string Multiply(Instr::Type type);
	static const std::string Divide(Instr::Type type);
	static const std::string Modulo(Instr::Type type);
	static const std::string Negate(Instr::Type type);
	static const std::string Not(Instr::Type type);
	static const std::string Increment(const std::string local, const int constant);
	static const std::string Decrement(const std::string local, const int constant);

private:
	static const std::string iAdd;
	static const std::string fAdd;
	static const std::string iSub;
	static const std::string fSub;
	static const std::string iMul;
	static const std::string fMul;
	static const std::string iDiv;
	static const std::string fDiv;
	static const std::string iRem;
	static const std::string iNeg;
	static const std::string fNeg;
	static const std::string bNot;
	static const std::string iInc;
	static const std::string iInc_1;
	static const std::string iDec;
	static const std::string iDec_1;
};

class CompInstr
{
public:
	static const std::string NotEqual(Instr::Type);
	static const std::string Equal(Instr::Type);
	static const std::string Less(Instr::Type);
	static const std::string LessEqual(Instr::Type);
	static const std::string Greater(Instr::Type);
	static const std::string GreaterEqual(Instr::Type);

private:
	static const std::string iNotEqual;
	static const std::string iEqual;
	static const std::string iLess;
	static const std::string iLessEqual;
	static const std::string iGreater;
	static const std::string iGreaterEqual;

	static const std::string fNotEqual;
	static const std::string fEqual;
	static const std::string fLess;
	static const std::string fLessEqual;
	static const std::string fGreater;
	static const std::string fGreaterEqual;

	static const std::string bNotEqual;
	static const std::string bEqual;
};

class CntrlFlwInstr
{
public:
	enum Scope
	{
		Current,
		Outer,
		Local,
		Global
	};

	static const std::string InitiateSub(CntrlFlwInstr::Scope scope, const int nestingLevels);
	static const std::string JumpSub(const int arguments, const int offset);
	static const std::string JumpSub(const int arguments, const std::string label);
	static const std::string JumpExtSub(const int index);
	static const std::string EnterSub(const int elements);
	static const std::string Return(Instr::Type type);
	static const std::string Jump(const int offset);
	static const std::string Jump(const std::string label);
	static const std::string Branch(bool condition, const int offset);
	static const std::string Branch(bool condition, const std::string label);

private:	
	static const std::string isr;
	static const std::string isrn;
	static const std::string isrl;
	static const std::string isrg;

	static const std::string jsr;
	static const std::string jsre;

	static const std::string esr;

	static const std::string iReturn;
	static const std::string fReturn;
	static const std::string bReturn;
	static const std::string vReturn;

	static const std::string jump;
	static const std::string branch_t;
	static const std::string branch_f;
};

class VarInstr
{
public:
	VarInstr();

	static const std::string LoadLocal(Instr::Type, const int index);	
	static const std::string LoadRelative(Instr::Type, const int levels, const int index);
	static const std::string LoadGlobal(Instr::Type, const int index);
	
	static const std::string StoreLocal(Instr::Type, const int index);
	static const std::string StoreRelative(Instr::Type, const int levels, const int index);
	static const std::string StoreGlobal(Instr::Type, const int index);
	
	static const std::string LoadConstant(const int value);
	static const std::string LoadConstant(const float value);
	static const std::string LoadConstant(const bool value);
	static const std::string GetConstantTable();

private:
	static const std::string LoadConstant(const std::string instr, const std::string value, const std::string type);

	static const std::string iLoad;
	static const std::string fLoad;	
	static const std::string bLoad;
	static const std::string aLoad;

	static const std::string iLoadN;
	static const std::string fLoadN;
	static const std::string bLoadN;
	static const std::string aLoadN;

	static const std::string iLoadG;
	static const std::string fLoadG;
	static const std::string bLoadG;
	static const std::string aLoadG;

	static const std::string iLoadC;
	static const std::string fLoadC;
	static const std::string bLoadCT;
	static const std::string bLoadCF;

	static const std::string iStore;
	static const std::string fStore;
	static const std::string bStore;
	static const std::string aStore;

	static const std::string iStoreN;
	static const std::string fStoreN;
	static const std::string bStoreN;

	static const std::string iStoreG;
	static const std::string fStoreG;
	static const std::string bStoreG;
	static const std::string aStoreG;

	static std::vector<std::tuple<std::string, std::string>> constants;
};

class ArrayInstr
{
public:
	static const std::string New(Instr::Type type, const int dimensions);
	static const std::string Read(Instr::Type type);
	static const std::string Store(Instr::Type type);
	static const std::string Size(const int dimensions);

private:
	static const std::string iNew;
	static const std::string fNew;
	static const std::string bNew;

	static const std::string size;

	static const std::string iRead;
	static const std::string fRead;
	static const std::string bRead;
	
	static const std::string iStore;
	static const std::string fStore;
	static const std::string bStore;
};

class CastInstr
{
public:
	static const std::string Int2Float();
	static const std::string Float2Int();
private:
	static const std::string int2Float;
	static const std::string float2Int;
};

class StackInstr
{
public:
	static const std::string Pop(Instr::Type);

private:
	static const std::string iPop;
	static const std::string fPop;
	static const std::string bPop;
};
