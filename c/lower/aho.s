gcc2_compiled.:
___gnu_compiled_c:
.text
	.align 8
LC0:
	.ascii "%d\12%d\12\0"
	.align 4
	.global _main
	.proc	04
_main:
	!#PROLOGUE# 0
	save %sp,-104,%sp
	!#PROLOGUE# 1
	call ___main,0
	nop
	call _tolower,0
	mov 52,%o0
	mov %o0,%o1
	sethi %hi(LC0),%o0
	or %o0,%lo(LC0),%o0
	call _printf,0
	mov 52,%o2
	ret
	restore
