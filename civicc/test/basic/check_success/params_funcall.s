foo:
	esr 0
	isrg
	jsr 0 foo
	ipop
	isrg
	jsr 0 foo
	iloadc_1
	iadd
	ireturn

; globals:
