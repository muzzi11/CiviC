foo:
	esr 9
	iloadc_0
	istore 0
	iloadc 0
	istore 1
	iloadc_1
	istore 2
0_for:
	iload_0
	iload_1
	ilt
	branch_f 0_end_for
	isrg
	jsr 0 foo
	isrg
	jsr 0 foo
	iload_0
	iload_2
	iadd
	istore 0
	jump 0_for
0_end_for:
	iloadc_0
	istore 3
	iloadc 0
	istore 4
	iloadc_1
	istore 5
1_for:
	iload_3
	iload 4
	ilt
	branch_f 1_end_for
	isrg
	jsr 0 foo
	iload_3
	iload 5
	iadd
	istore 3
	jump 1_for
1_end_for:
	iloadc 0
	istore 6
	iloadc_0
	istore 7
	iloadc_1
	ineg
	istore 8
2_for:
	iload 6
	iload 7
	ilt
	branch_f 2_end_for
	isrg
	jsr 0 foo
	iload 6
	iload 8
	iadd
	istore 6
	jump 2_for
2_end_for:
	return

; globals:
.const int 10
