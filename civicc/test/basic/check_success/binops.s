foo:
	esr 6
	iloadc 0
	istore 1
	iloadc 1
	iloadc 2
	iadd
	istore 2
	iloadc 3
	iloadc 4
	imul
	istore 3
	iloadc 5
	iloadc 6
	irem
	istore 4
	iload_2
	iload_3
	iadd
	iload 4
	iload_1
	imul
	isub
	istore 5
	return

; globals:
.const int 2
.const int 3
.const int 4
.const int 5
.const int 6
.const int 7
.const int 8
