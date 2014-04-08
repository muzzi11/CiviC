f:
	esr 23
	iloadc 0
	iloadc 1
	iadd
	iloadc 2
	iadd
	iloadc 3
	iadd
	istore 0
	floadc 4
	floadc 5
	fadd
	floadc 6
	fadd
	fstore 1
	bloadc_f
	branch_f 0_false_expr
	bloadc_t
	jump 0_end
0_false_expr:
	bloadc_t
0_end:
	bloadc_f
	branch_f 1_false_expr
	bloadc_f
	branch_f 2_false_expr
	bloadc_t
	jump 2_end
2_false_expr:
	bloadc_t
2_end:
	jump 1_end
1_false_expr:
	bloadc_f
1_end:
	bstore 2
	iloadc 0
	iloadc 1
	isub
	iloadc 2
	isub
	iloadc 3
	isub
	istore 3
	floadc 4
	floadc 5
	fsub
	floadc 6
	fsub
	fstore 4
	iloadc 0
	iloadc 1
	imul
	iloadc_1
	imul
	istore 5
	floadc 7
	floadc 8
	fmul
	fstore 6
	bloadc_f
	branch_f 3_false_expr
	bloadc_f
	jump 3_end
3_false_expr:
	bloadc_f
3_end:
	bloadc_t
	branch_f 4_false_expr
	bloadc_f
	jump 4_end
4_false_expr:
	bloadc_f
	branch_f 5_false_expr
	bloadc_f
	jump 5_end
5_false_expr:
	bloadc_f
5_end:
4_end:
	bstore 7
	iloadc 2
	iloadc 1
	idiv
	istore 8
	iloadc 9
	iloadc_0
	idiv
	istore 9
	floadc 10
	floadc 8
	fdiv
	fstore 10
	floadc 11
	floadc_0
	fdiv
	fstore 11
	iloadc 12
	iloadc 13
	irem
	iloadc 1
	irem
	istore 12
	iloadc 14
	iloadc 14
	ilt
	bstore 13
	floadc 5
	floadc 15
	flt
	bstore 14
	iloadc 14
	iloadc 14
	ile
	bstore 15
	floadc 5
	floadc 15
	fle
	bstore 16
	iloadc 16
	iloadc 17
	ilt
	iloadc 18
	iloadc 19
	igt
	iloadc 16
	iloadc 17
	ilt
	branch_f 6_false_expr
	bloadc_t
	jump 6_end
6_false_expr:
	iloadc 18
	iloadc 19
	igt
6_end:
	bstore 17
	floadc 5
	floadc 8
	igt
	bstore 18
	iloadc 13
	iloadc 13
	ine
	bstore 19
	iloadc 13
	iloadc 13
	ieq
	bstore 20
	floadc 20
	floadc 21
	fne
	bstore 21
	bloadc_t
	bloadc_f
	bne
	bstore 22
	return

; globals:
.const int 2
.const int 3
.const int 4
.const int 5
.const float 39
.const float 3
.const float 21
.const float 3.14
.const float 2
.const int 6
.const float 5
.const float 9
.const int 12
.const int 8
.const int 20
.const float 6.1
.const int 60
.const int 50
.const int 30
.const int 10
.const float 1.2
.const float 2.1
