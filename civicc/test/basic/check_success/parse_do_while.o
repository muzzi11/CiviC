foo:
	esr 0
0_do_while:
	isrg
	jsr 0 foo
	isrg
	jsr 0 foo
	bloadc_t
branch_t 0_do_while
1_do_while:
	isrg
	jsr 0 foo
	bloadc_f
branch_t 1_do_while
	return

; globals:
