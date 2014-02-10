Gcd:
	esr 1
1:
	iload 1
	iloadc 0
	ine
	branch_f 2
	iload 1
	istore 2
	iload 0
	iload 1
	irem
	istore 1
	iload 2
	istore 0
	jump 1
2:
	iload 0
	ireturn

Factorial:
	esr 1
	iloadc 1
	istore 1
3:
	iload 0
	iloadc 1
	igt
	branch_f 4
	iload 1
	iload 0
	imul
	istore 1
	iload 0
	iloadc 1
	isub
	istore 0
	jump 3
4:
	iload 1
	ireturn

Fibonacci:
	esr 5
	iloadc 0
	istore 1
	iloadc 1
	istore 2
	iloadc 0
	istore 4
	iload 0
	iloadc 1
	iadd
	istore 5
5:
	iload 4
	iload 5
	ilt
	branch_f 6
	isrg
	iload 1
	jsre 0
	isrg
	iloadc 1
	jsre 4
	iload 1
	iload 2
	iadd
	istore 3
	iload 2
	istore 1
	iload 3
	istore 2
	iload 4
	iloadc 1
	iadd
	istore 4
	jump 5
6:
	return

Primes:
	esr 3
	iloadc 2
	istore 1
	iloadc 0
	istore 2
	iload 0
	istore 3
7:
	iload 2
	iload 3
	ilt
	branch_f 8
9:
	isrl
	iload 1
	jsr 1 Primes$IsPrime
	bnot
	branch_f 10
	iload 1
	iloadc 1
	iadd
	istore 1
	jump 9
10:
	isrg
	iload 1
	jsre 0
	isrg
	iloadc 1
	jsre 4
	iload 1
	iloadc 1
	iadd
	istore 1
	iload 2
	iloadc 1
	iadd
	istore 2
	jump 7
8:
	return

Primes$IsPrime:
	esr 2
	iloadc 2
	istore 1
	bloadc 3
	bstore 2
11:
	iload 1
	iload 0
	ilt
	branch_f 13
	bload 2
	jump 14
13:
	bloadc 4
14:
	branch_f 12
	iload 0
	iload 1
	irem
	iloadc 0
	ieq
	branch_f 15
	bloadc 4
	bstore 2
	jump 16
15:
16:
	iload 1
	iloadc 1
	iadd
	istore 1
	jump 11
12:
	bload 2
	breturn

PrintMatrix:
	esr 6
	aload 0
	asize 0
	istore 1
	aload 0
	asize 1
	istore 2
	iloadc 0
	istore 5
	iload 1
	istore 6
17:
	iload 5
	iload 6
	ilt
	branch_f 18
	iloadc 0
	istore 3
	iload 2
	istore 4
19:
	iload 3
	iload 4
	ilt
	branch_f 20
	isrg
	iload 3
	iload 5
	aload 0
	floada
	jsre 1
	isrg
	iloadc 1
	jsre 4
	iload 3
	iloadc 1
	iadd
	istore 3
	jump 19
20:
	isrg
	iloadc 1
	jsre 5
	iload 5
	iloadc 1
	iadd
	istore 5
	jump 17
18:
	return

ScanMatrix:
	esr 6
	aload 0
	asize 0
	istore 1
	aload 0
	asize 1
	istore 2
	iloadc 0
	istore 5
	iload 1
	istore 6
21:
	iload 5
	iload 6
	ilt
	branch_f 22
	iloadc 0
	istore 3
	iload 2
	istore 4
23:
	iload 3
	iload 4
	ilt
	branch_f 24
	isrg
	jsre 3
	iload 3
	iload 5
	aload 0
	fstorea
	iload 3
	iloadc 1
	iadd
	istore 3
	jump 23
24:
	iload 5
	iloadc 1
	iadd
	istore 5
	jump 21
22:
	return

MatrixMul:
	esr 12
	aload 0
	asize 0
	istore 3
	aload 0
	asize 1
	istore 4
	aload 1
	asize 0
	istore 5
	aload 1
	asize 1
	istore 6
	aload 2
	asize 0
	istore 7
	aload 2
	asize 1
	istore 8
	iload 4
	iload 5
	ieq
	branch_f 25
	iload 3
	iload 7
	ieq
	jump 26
25:
	bloadc 4
26:
	branch_f 27
	iload 6
	iload 8
	ieq
	jump 28
27:
	bloadc 4
28:
	branch_f 29
	iloadc 0
	istore 13
	iload 7
	istore 14
31:
	iload 13
	iload 14
	ilt
	branch_f 32
	iloadc 0
	istore 11
	iload 8
	istore 12
33:
	iload 11
	iload 12
	ilt
	branch_f 34
	floadc 5
	iload 11
	iload 13
	aload 2
	fstorea
	iloadc 0
	istore 9
	iload 4
	istore 10
35:
	iload 9
	iload 10
	ilt
	branch_f 36
	iload 11
	iload 13
	aload 2
	floada
	iload 9
	iload 13
	aload 0
	floada
	iload 11
	iload 9
	aload 1
	floada
	fmul
	fadd
	iload 11
	iload 13
	aload 2
	fstorea
	iload 9
	iloadc 1
	iadd
	istore 9
	jump 35
36:
	iload 11
	iloadc 1
	iadd
	istore 11
	jump 33
34:
	iload 13
	iloadc 1
	iadd
	istore 13
	jump 31
32:
	jump 30
29:
30:
	return

PrintVector:
	esr 3
	aload 0
	asize 0
	istore 1
	iloadc 0
	istore 2
	iload 1
	istore 3
37:
	iload 2
	iload 3
	ilt
	branch_f 38
	isrg
	iload 2
	aload 0
	iloada
	jsre 0
	isrg
	iloadc 1
	jsre 4
	iload 2
	iloadc 1
	iadd
	istore 2
	jump 37
38:
	isrg
	iloadc 1
	jsre 5
	return

ScanVector:
	esr 3
	aload 0
	asize 0
	istore 1
	iloadc 0
	istore 2
	iload 1
	istore 3
39:
	iload 2
	iload 3
	ilt
	branch_f 40
	isrg
	jsre 2
	iload 2
	aload 0
	istorea
	iload 2
	iloadc 1
	iadd
	istore 2
	jump 39
40:
	return

Quicksort:
	esr 1
	aload 0
	asize 0
	istore 1
	isrl
	aload 0
	iload 1
	jsr 2 Quicksort$Recursion
	return

Quicksort$Lesseq:
	esr 5
	aload 1
	asize 0
	istore 4
	aload 3
	asize 0
	istore 5
	iloadc 0
	istore 6
	iloadc 0
	istore 7
	iload 2
	istore 8
41:
	iload 7
	iload 8
	ilt
	branch_f 42
	iload 7
	iload 0
	ine
	branch_f 43
	iload 7
	aload 1
	iloada
	iload 0
	aload 1
	iloada
	ile
	jump 44
43:
	bloadc 4
44:
	branch_f 45
	iload 7
	aload 1
	iloada
	iload 6
	aload 3
	istorea
	iload 6
	iloadc 1
	iadd
	istore 6
	jump 46
45:
46:
	iload 7
	iloadc 1
	iadd
	istore 7
	jump 41
42:
	iload 6
	ireturn

Quicksort$Greater:
	esr 5
	aload 1
	asize 0
	istore 4
	aload 3
	asize 0
	istore 5
	iloadc 0
	istore 6
	iloadc 0
	istore 7
	iload 2
	istore 8
47:
	iload 7
	iload 8
	ilt
	branch_f 48
	iload 7
	iload 0
	ine
	branch_f 49
	iload 7
	aload 1
	iloada
	iload 0
	aload 1
	iloada
	igt
	jump 50
49:
	bloadc 4
50:
	branch_f 51
	iload 7
	aload 1
	iloada
	iload 6
	aload 3
	istorea
	iload 6
	iloadc 1
	iadd
	istore 6
	jump 52
51:
52:
	iload 7
	iloadc 1
	iadd
	istore 7
	jump 47
48:
	iload 6
	ireturn

Quicksort$Concatenate:
	esr 7
	aload 0
	asize 0
	istore 5
	aload 2
	asize 0
	istore 6
	aload 4
	asize 0
	istore 7
	iloadc 0
	istore 8
	iload 1
	istore 9
53:
	iload 8
	iload 9
	ilt
	branch_f 54
	iload 8
	aload 0
	iloada
	iload 8
	aload 4
	istorea
	iload 8
	iloadc 1
	iadd
	istore 8
	jump 53
54:
	iloadc 0
	istore 10
	iload 3
	istore 11
55:
	iload 10
	iload 11
	ilt
	branch_f 56
	iload 10
	aload 2
	iloada
	iload 1
	iload 10
	iadd
	aload 4
	istorea
	iload 10
	iloadc 1
	iadd
	istore 10
	jump 55
56:
	return

Quicksort$Recursion:
	esr 6
	aload 0
	asize 0
	istore 2
	iload 2
	inewa 1
	astore 6
	iload 2
	inewa 1
	astore 7
	iload 1
	iloadc 1
	igt
	branch_f 57
	iload 1
	iloadc 2
	idiv
	istore 5
	isr
	iload 5
	aload 0
	iload 1
	aload 6
	jsr 4 Quicksort$Lesseq
	istore 3
	isr
	iload 5
	aload 0
	iload 1
	aload 7
	jsr 4 Quicksort$Greater
	istore 4
	isr
	aload 6
	iload 3
	jsr 2 Quicksort$Recursion
	isr
	aload 7
	iload 4
	jsr 2 Quicksort$Recursion
	iload 5
	aload 0
	iloada
	iload 3
	aload 6
	istorea
	isr
	aload 6
	iload 3
	iloadc 1
	iadd
	aload 7
	iload 4
	aload 0
	jsr 5 Quicksort$Concatenate
	jump 58
57:
58:
	return

main:
	esr 4
	iloadc 6
	inewa 1
	astore 0
	iloadc 2
	iloadc 2
	fnewa 2
	astore 1
	iloadc 2
	iloadc 2
	fnewa 2
	astore 2
	iloadc 2
	iloadc 2
	fnewa 2
	astore 3
	isrg
	aload 0
	jsr 1 ScanVector
	isrg
	aload 0
	jsr 1 Quicksort
	isrg
	aload 0
	jsr 1 PrintVector
	isrg
	iloadc 1
	jsre 5
	iloadc 0
	ireturn

__init:
	esr 0
	return

.import "printInt" void int
.import "printFloat" void float
.import "scanInt" int
.import "scanFloat" float
.import "printSpaces" void int
.import "printNewlines" void int
.export "main" int main
.export "__init" void __init
.const int 0
.const int 1
.const int 2
.const bool true
.const bool false
.const float 0
.const int 5

