#include "node.h"


int BaseNode::familyCounter_ = 0;
std::unordered_map<int, std::string> BaseNode::familyNames_(1);

int BaseNode::Family() const
{
	return family_;
}

std::string BaseNode::FamilyName() const
{
	return familyNames_[family_];
}
