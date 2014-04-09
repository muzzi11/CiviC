test_do_while2:
	esr 2
	iloadc_0
	istore 0
	iloadc 0
	istore 1
0_do_while:
	iload_0
	iloadc_1
	iadd
	istore 0
	iload_1
	iloadc_1
	isub
	istore 1
	iload_0
	iload_1
	ile
branch_t 0_do_while
	return

; globals:
.const int 10
