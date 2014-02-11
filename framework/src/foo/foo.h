/*
Created by Jos Bonsink & Mustafa Karaalioglu
*/

#ifndef _FOO_H_
#define _FOO_H_

#include "types.h"

extern node* FOOvar(node* arg_node, info* arg_info);
extern node* FOOvarlet(node* arg_node, info* arg_info);
extern node* FOObinop(node* arg_node, info* arg_info);

extern node* FOOdoFoo(node* syntaxtree);

#endif /* _SAC_FOO_H_ */
