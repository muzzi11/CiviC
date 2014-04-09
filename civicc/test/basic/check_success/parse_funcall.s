foo:
	esr 0
	isrg
	jsr 0 foo
	return

; globals:
