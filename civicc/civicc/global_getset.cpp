#include "global_getset.h"
#include "node.h"
#include "traverse.h"

using namespace Nodes;


void CreateGettersSetters(NodePtr root)
{
	// export
	size_t size = root->children.size();
	for(size_t i = 0; i < size; ++i)
	{
		auto global = StaticCast<GlobalDef>(root->children[i]);
		if(global)
		{
			if(!global->exp) continue;

			auto get = std::make_shared<FunctionDef>();
			auto set = std::make_shared<FunctionDef>();
			auto id = std::make_shared<Identifier>(global->var.name);
			auto assign = std::make_shared<Assignment>(global->var.name);
			auto ret = std::make_shared<Return>();

			id->type = assign->type = global->var.type;
			id->dec = assign->dec = global;	

			get->exp = set->exp = true;

			get->header.returnType = ret->type = global->var.type;
			get->header.name = ret->functionName = "_get_" + global->var.name;
			get->children.push_back(ret);
			ret->children.push_back(id);

			set->header.returnType = Type::Void;
			set->header.name = "_set_" + global->var.name;
			set->header.params.push_back({ global->var.type, "v" });
			set->children.push_back(assign);
			assign->children.push_back(id);

			root->children.push_back(get);
			root->children.push_back(set);
		}
	}

	// extern
	for(size_t i = 0; i < size; ++i)
	{
		auto globalDec = StaticCast<GlobalDec>(root->children[i]);
		if(globalDec)
		{
			auto getter = std::make_shared<FunctionDec>();
			getter->header.name = "_get_" + globalDec->param.name;
			getter->header.returnType = globalDec->param.type;
			globalDec->getter = getter;
			root->children.push_back(getter);

			auto setter = std::make_shared<FunctionDec>();
			setter->header.name = "_set_" + globalDec->param.name;
			setter->header.returnType = Type::Void;
			setter->header.params.push_back({ globalDec->param.type, "v" });
			globalDec->setter = setter;
			root->children.push_back(setter);
		}
	}

	Replace<Assignment>(root, [](std::shared_ptr<Assignment> assign) -> NodePtr
	{
		auto globalDec = StaticCast<GlobalDec>(assign->dec);
		if(globalDec)
		{
			auto call = std::make_shared<Call>();

			call->name = globalDec->setter->header.name;
			call->dec = globalDec->setter;
			call->children.swap(assign->children);

			return call;
		}

		return assign;
	});

	Replace<Identifier>(root, [](std::shared_ptr<Identifier> id) -> NodePtr
	{
		auto globalDec = StaticCast<GlobalDec>(id->dec);
		if(globalDec)
		{
			auto call = std::make_shared<Call>();

			call->name = globalDec->getter->header.name;
			call->dec = globalDec->getter;	

			return call;
		}

		return id;
	});
}