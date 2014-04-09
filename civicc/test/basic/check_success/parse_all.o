bfoo:
	esr 0
	bloadc_t
	breturn
ifoo:
	esr 0
	iloadc_1
	ireturn
ffoo:
	esr 0
	floadc_1
	freturn
vfoo:
	esr 0
	return
bar:
	esr 0
	iload_0
	fload_1
	f2i
	iadd
	ireturn
baz:
	esr 0
	return
main:
	esr 15
	floadc 0
	fstore 5
	bloadc_f
	bstore 0
	isrg
	jsr 0 vfoo
	isrg
	jsr 0 bfoo
	branch_f 0_else
	iloadc_1
	istore 1
0_else:
	isrg
	jsr 0 bfoo
	branch_f 1_else
	iloadc_1
	istore 1
1_else:
	isrg
	jsr 0 bfoo
	branch_f 2_else
	iloadc 1
	istore 1
	jump 2_end
2_else:
	iloadc 2
	istore 1
2_end:
	isrg
	jsr 0 bfoo
	branch_f 3_else
	iloadc 1
	istore 1
	jump 3_end
3_else:
	iloadc 2
	istore 1
3_end:
	isrg
	jsr 0 bfoo
	branch_f 4_else
	iloadc 1
	istore 1
	jump 4_end
4_else:
	isrg
	jsr 0 bfoo
	branch_f 5_else
	iloadc 2
	istore 1
	jump 5_end
5_else:
	iloadc 3
	istore 1
5_end:
4_end:
	isrg
	jsr 0 bfoo
	branch_f 6_else
7_do_while:
	isrg
	jsr 0 vfoo
	isrg
	jsr 0 bfoo
branch_t 7_do_while
6_else:
	isrg
	jsr 0 bfoo
	branch_f 8_else
9_do_while:
	isrg
	jsr 0 vfoo
	isrg
	jsr 0 bfoo
branch_t 9_do_while
8_else:
10_do_while:
	isrg
	jsr 0 vfoo
	isrg
	jsr 0 bfoo
branch_t 10_do_while
11_do_while:
	isrg
	jsr 0 vfoo
	isrg
	jsr 0 bfoo
branch_t 11_do_while
	iloadc_0
	istore 6
	iloadc 4
	istore 7
	iloadc_1
	istore 8
12_for:
	iload 6
	iload 7
	ilt
	branch_f 12_end_for
	isrg
	jsr 0 vfoo
	iload 6
	iload 8
	iadd
	istore 6
	jump 12_for
12_end_for:
	iloadc_0
	istore 9
	iloadc 4
	istore 10
	iloadc 1
	istore 11
13_for:
	iload 9
	iload 10
	ilt
	branch_f 13_end_for
	isrg
	jsr 0 vfoo
	iload 9
	iload 11
	iadd
	istore 9
	jump 13_for
13_end_for:
	iload_2
	istore 12
	iload_3
	istore 13
	iload 4
	istore 14
14_for:
	iload 12
	iload 13
	ilt
	branch_f 14_end_for
	isrg
	iload 12
	iloadc_0
	bne
	jsr 1 baz
	iload 12
	iload 14
	iadd
	istore 12
	jump 14_for
14_end_for:
	iload_2
	iload_3
	iload 4
	imul
	iload_1
	irem
	iload 4
	idiv
	iadd
	iloadc 4
	ineg
	isub
	istore 1
	iload_1
	iloadc_0
	ilt
	iload_1
	iloadc 5
	igt
	iload_1
	iloadc_0
	ilt
	branch_f 16_false_expr
	bloadc_t
	jump 16_end
16_false_expr:
	iload_1
	iloadc 5
	igt
16_end:
	branch_f 15_else
	iload_1
	iloadc_0
	ige
	iload_1
	iloadc 5
	ile
	iload_1
	iloadc_0
	ige
	branch_f 18_false_expr
	iload_1
	iloadc 5
	ile
	jump 18_end
18_false_expr:
	bloadc_f
18_end:
	iload_1
	iloadc_0
	ine
	iload_1
	iloadc_0
	ige
	iload_1
	iloadc 5
	ile
	iload_1
	iloadc_0
	ige
	branch_f 20_false_expr
	iload_1
	iloadc 5
	ile
	jump 20_end
20_false_expr:
	bloadc_f
20_end:
	branch_f 19_false_expr
	bloadc_t
	jump 19_end
19_false_expr:
	iload_1
	iloadc_0
	ine
19_end:
	iload_1
	iloadc 5
	ine
	iload_1
	iloadc_0
	ige
	iload_1
	iloadc 5
	ile
	iload_1
	iloadc_0
	ige
	branch_f 22_false_expr
	iload_1
	iloadc 5
	ile
	jump 22_end
22_false_expr:
	bloadc_f
22_end:
	iload_1
	iloadc_0
	ine
	iload_1
	iloadc_0
	ige
	iload_1
	iloadc 5
	ile
	iload_1
	iloadc_0
	ige
	branch_f 24_false_expr
	iload_1
	iloadc 5
	ile
	jump 24_end
24_false_expr:
	bloadc_f
24_end:
	branch_f 23_false_expr
	bloadc_t
	jump 23_end
23_false_expr:
	iload_1
	iloadc_0
	ine
23_end:
	branch_f 21_false_expr
	iload_1
	iloadc 5
	ine
	jump 21_end
21_false_expr:
	bloadc_f
21_end:
	branch_f 17_else
	iload_1
	ineg
	istore 1
17_else:
15_else:
	iloadc_0
	ireturn

; globals:
.const float 2.5
.const int 2
.const int 3
.const int 4
.const int 10
.const int 100
.import "printInt" void int
.import "_get_ext" int
.import "_set_ext" void int
.export "main" int main
