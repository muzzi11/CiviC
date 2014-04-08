__init:
	esr 0
	iloadc 0
	istoreg 4
	floadc_1
	fstoreg 5
	bloadc_f
	bstoreg 6
	iloadc 0
	istoreg 10
	floadc_1
	fstoreg 11
	bloadc_f
	bstoreg 12
	return
_get_a2:
	esr 0
	iloadg 7
	ireturn
_set_a2:
	esr 0
	iloadg 7
	istoreg 7
	return
_get_b2:
	esr 0
	floadg 8
	freturn
_set_b2:
	esr 0
	floadg 8
	fstoreg 8
	return
_get_c2:
	esr 0
	bloadg 9
	breturn
_set_c2:
	esr 0
	bloadg 9
	bstoreg 9
	return
_get_d2:
	esr 0
	iloadg 10
	ireturn
_set_d2:
	esr 0
	iloadg 10
	istoreg 10
	return
_get_e2:
	esr 0
	floadg 11
	freturn
_set_e2:
	esr 0
	floadg 11
	fstoreg 11
	return
_get_f2:
	esr 0
	bloadg 12
	breturn
_set_f2:
	esr 0
	bloadg 12
	bstoreg 12
	return

; globals:
.const int 321
.global int
.global float
.global bool
.global int
.global float
.global bool
.global int
.global float
.global bool
.global int
.global float
.global bool
.export "_get_a2" int _get_a2
.export "_set_a2" void int _set_a2
.export "_get_b2" float _get_b2
.export "_set_b2" void float _set_b2
.export "_get_c2" bool _get_c2
.export "_set_c2" void bool _set_c2
.export "_get_d2" int _get_d2
.export "_set_d2" void int _set_d2
.export "_get_e2" float _get_e2
.export "_set_e2" void float _set_e2
.export "_get_f2" bool _get_f2
.export "_set_f2" void bool _set_f2
