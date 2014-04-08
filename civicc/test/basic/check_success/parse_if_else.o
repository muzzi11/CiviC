foo:
	esr 0
	bloadc_t
	branch_f 0_else
	isrg
	jsr 0 foo
	jump 0_end
0_else:
	isrg
	jsr 0 foo
0_end:
	bloadc_f
	branch_f 1_else
	isrg
	jsr 0 foo
	jump 1_end
1_else:
	isrg
	jsr 0 foo
1_end:
	bloadc_t
	branch_f 2_else
	isrg
	jsr 0 foo
	jump 2_end
2_else:
	isrg
	jsr 0 foo
2_end:
	return

; globals:
