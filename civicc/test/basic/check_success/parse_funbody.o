vardec:
	esr 1
	return
vardec_stat:
	esr 1
	iloadc 0
	istore 1
	return
vardec_ret:
	esr 1
	iloadc 0
	ireturn
vardec_stat_ret:
	esr 1
	iloadc 0
	istore 3
	iload_3
	ireturn
stat_ret:
	esr 0
	isrg
	jsr 0 vardec
	iloadc 0
	ireturn
ret:
	esr 0
	iloadc 0
	ireturn

; globals:
.const int 2
