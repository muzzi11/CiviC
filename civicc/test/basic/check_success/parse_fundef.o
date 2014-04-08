vfoo:
	esr 0
	return
bfoo:
	esr 0
	bloadc_t
	breturn
ifoo:
	esr 0
	iload_0
	ireturn
ffoo:
	esr 0
	fload_0
	fload_1
	fadd
	freturn
exported_fun:
	esr 0
	iloadc_1
	ireturn

; globals:
.export "exported_fun" int exported_fun
