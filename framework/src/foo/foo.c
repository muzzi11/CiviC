/*
Created by Jos Bonsink & Mustafa Karaalioglu
*/

#include "foo.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "str.h"
#include "memory.h"
// #include "internal_lib.h"

struct INFO
{
  int operatorCount;
};

static info* MakeInfo()
{
  info* result = MEMmalloc(sizeof(info));
  
  result->operatorCount = 0;
  
  return result;
}

node* FOOvar(node* arg_node, info* arg_info)
{
  char* temp = VAR_NAME(arg_node);
  
  VAR_NAME(arg_node) = STRcat("__", temp);
  MEMfree(temp);
  
  return arg_node;
}

node* FOOvarlet(node* arg_node, info* arg_info)
{
  char* temp = VARLET_NAME(arg_node);
  
  VARLET_NAME(arg_node) = STRcat("__", temp);
  MEMfree(temp);
  
  return arg_node;
}

node* FOObinop(node* arg_node, info* arg_info)
{
  switch(BINOP_OP(arg_node))
  {
  case BO_add:
  case BO_sub:
  case BO_mul:
  case BO_div:
  case BO_mod:
  	arg_info->operatorCount++;
  	break;
  	
  default:
    break;
  }
  
  BINOP_LEFT( arg_node) = TRAVdo( BINOP_LEFT( arg_node), arg_info);
  
  BINOP_RIGHT( arg_node) = TRAVdo( BINOP_RIGHT( arg_node), arg_info);
  
  return arg_node;
}

node* FOOdoFoo(node* syntaxtree)
{
  info *info;
  info = MakeInfo();
  
  TRAVpush(TR_foo);

  syntaxtree = TRAVdo(syntaxtree, info);

  TRAVpop();
  
  printf("\n\nNumber of arithmetic operators: %d\n\n", info->operatorCount);
  
  info = MEMfree(info);

  return syntaxtree;
}
