#pragma once

#include "node.h"

void SeperateForLoopInduction(Node::NodePtr root);
void SeperateVarDecFromInit(Node::NodePtr root);
void SeperateGlobalDefFromInit(Node::NodePtr root);