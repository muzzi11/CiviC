foo:
	esr 0
	bloadc_t
	branch_f 0_else
	isrg
	jsr 0 foo
	isrg
	jsr 0 foo
0_else:
	bloadc_f
	branch_f 1_else
	isrg
	jsr 0 foo
1_else:
	return

; globals:
