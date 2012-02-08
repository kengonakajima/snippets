	.file	"aho.c"
	.version	"01.01"
gcc2_compiled.:
	nop
		.section	.rodata
.LC0:
	.string	"aho\n"
.text
	.align 16
.globl hoge
	.type	 hoge,@function
hoge:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	call	mcount
	subl	$12, %esp
	pushl	$.LC0
	call	printf
	addl	$16, %esp
	movl	%ebp, %esp
	popl	%ebp
	ret
.Lfe1:
	.size	 hoge,.Lfe1-hoge
	.align 16
.globl main
	.type	 main,@function
main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	call	mcount
	call	hoge
	movl	%ebp, %esp
	popl	%ebp
	ret
.Lfe2:
	.size	 main,.Lfe2-main
	.ident	"GCC: (GNU) 2.96 20000731 (Kondara MNU/Linux 2.0)"
