foo:
	esr 0
	isrg
	iloadc 0
	jsre 0
	isrl
	jsr 0 foo__bar
	isrg
	jsr 0 baz
	return
bar:
	esr 0
	isrg
	iloadc 1
	jsre 0
	return
baz:
	esr 0
	isrg
	iloadc 2
	jsre 0
	isrg
	jsr 0 bar
	return
main:
	esr 0
	isrg
	jsr 0 foo
	isrg
	jsr 0 bar
	isrg
	jsr 0 baz
	isrg
	iloadc_1
	jsre 1
	iloadc_0
	ireturn
foo__bar:
	esr 0
	isrg
	iloadc 3
	jsre 0
	isrl
	jsr 0 foo__bar
	isrg
	jsr 0 baz
	isrn 0
	jsr 0 foo__foobar
	return
foo__foobar:
	esr 0
	return
foo__bar__bar:
	esr 0
	isrg
	iloadc_1
	jsre 0
	isrn 1
	jsr 0 foo__foobar
	return

; globals:
.const int 3
.const int 4
.const int 5
.const int 2
.import "printInt" void int
.import "printNewlines" void int
.export "main" int main
