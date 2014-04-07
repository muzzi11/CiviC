#include "global_getset.h"
#include "node.h"
#include "traverse.h"

using namespace Nodes;


void CreateGettersSetters(NodePtr root)
{
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

	/*Replace<Assignment>(root, [](std::shared_ptr<Assignment> assign)
	{
		return assign;
	});*/
}