#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "node.h"

namespace SymbolTable
{
	class Record
	{
	public:
		Record() { }
		Record(bool immutable, Nodes::Type node, Nodes::NodePtr ptr) : immutable(immutable), initialized(false), type(node), decPtr(ptr) { }

		bool immutable;
		bool initialized;
		Nodes::Type type;
		Nodes::NodePtr decPtr;
		std::vector<Nodes::Param> params;

		std::vector<int> arrayDimensions;
		std::vector<std::string> dim;
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
	
	class Sheaf
	{
	public:
		Sheaf() : level(-1) { }
		void InitializeScope();
		void FinalizeScope();

		Record* LookUp(const std::string name);
		bool Insert(const std::string name, const Record record);

	private:
		int level;
		std::unordered_map<int, Table> tables;
	};
		



};
