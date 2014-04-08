#include <unordered_map>
#include <string>
#include <vector>

#include "node.h"
#include "symboltable.h"
#include "assert.h"

void SymbolTable::Sheaf::InitializeScope()
{
	++level;

	if (tables.size() == 0 || tables.find(level) == tables.end())
		tables[level] = Table();
	
	tables[level].Clear();
}

void SymbolTable::Sheaf::FinalizeScope()
{
	if (level > 0) --level;
}

SymbolTable::Record* SymbolTable::Sheaf::LookUp(const std::string name)
{
	for (int i = level; i >= 0; --i)
	{
		auto& table = Sheaf::tables[i];
		Record* record = table.LookUp(name);
		if (record != nullptr) return record;
	}
	return nullptr;
}

bool SymbolTable::Sheaf::Insert(std::string name, Record record)
{
	return tables[level].Insert(name, record);
}

SymbolTable::Record* SymbolTable::Table::LookUp(std::string name)
{
	auto& it = records.find(name);
	if (it == records.end()) return nullptr;
	return &(it->second);
}

bool SymbolTable::Table::Insert(std::string name, SymbolTable::Record record)
{
	auto it = records.find(name);
	if (it != records.end()) return false;
	records[name] = record;
	return true;
}

void SymbolTable::Table::Clear()
{
	records.clear();
}