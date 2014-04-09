foo:
	esr 0
	bloadc_t
	branch_f 0_else
1_do_while:
	isrg
	jsr 0 foo
	isrg
	jsr 0 foo
	bloadc_t
branch_t 1_do_while
0_else:
	bloadc_t
	branch_f 2_else
3_do_while:
	isrg
	jsr 0 foo
	bloadc_t
branch_t 3_do_while
2_else:
	return

; globals:
