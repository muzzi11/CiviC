#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "node.h"

namespace SymbolTable
{	
	class Record;
	class FuncArg;
	class Table;

	class Sheaf
	{
	public:
		Sheaf() : level(-1) { }
		void InitializeScope();
		void FinalizeScope();

		Record* LookUp(const std::string name);
		Record* LookUp(const std::string name, const int level, const int scope);
		bool Insert(const std::string name, const Record record);

	private:
		int level;
		std::unordered_map<int, Table> tables;
	};
		
	class Table
	{
	public:
		Table() { }

		Record* LookUp(const std::string name);
		bool Insert(const std::string name, const Record record);
		void Clear();

	private:
		std::unordered_map<std::string, Record> records;
	};

	class Record
	{
	public:
		Record() { }
		Record(bool immutable, Node::Type node, Node::NodePtr ptr) : immutable(immutable), type(node), initialized(false), decPtr(ptr) { }

		bool immutable;
		bool initialized;
		Node::Type type;
		Node::NodePtr decPtr;
		std::vector<Node::Type> funcArgs;
		std::vector<int> arrayDimensions;
	};
};
