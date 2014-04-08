foo:
	esr 2
	iloadc_1
	iloadc 0
	iadd
	istore 0
	iload_0
	iloadc_1
	isub
	istore 0
	iload_0
	iloadc 1
	imul
	istore 0
	iload_0
	iloadc 2
	idiv
	istore 0
	iload_0
	iloadc 3
	irem
	istore 0
	iload_0
	iloadc_1
	ilt
	bstore 1
	iload_0
	iloadc 0
	igt
	bstore 1
	iload_0
	iloadc 4
	ile
	bstore 1
	iload_0
	iloadc 3
	ige
	bstore 1
	iload_0
	iloadc 5
	ieq
	bstore 1
	iload_0
	iloadc 6
	ine
	bstore 1
	iload_0
	iloadc 6
	ine
	iload_0
	iloadc 3
	ige
	iload_0
	iloadc 6
	ine
	branch_f 0_false_expr
	iload_0
	iloadc 3
	ige
	jump 0_end
0_false_expr:
	bloadc_f
0_end:
	bstore 1
	iload_0
	iloadc 6
	ieq
	iload_0
	iloadc 3
	ile
	iload_0
	iloadc 6
	ieq
	branch_f 1_false_expr
	bloadc_t
	jump 1_end
1_false_expr:
	iload_0
	iloadc 3
	ile
1_end:
	bstore 1
	iload_0
	ineg
	istore 0
	bload_1
	bnot
	bstore 1
	iload_0
	iloadc_1
	ineg
	isub
	istore 0
	iload_0
	ineg
	ineg
	ineg
	ineg
	ineg
	ineg
	istore 0
	return

; globals:
.const int 2
.const int 45
.const int 9
.const int 4
.const int 3
.const int 5
.const int 6
