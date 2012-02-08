	.file	"aho.c"
	.version	"01.01"
gcc2_compiled.:
.text
	.align 16
.globl hoge
	.type	 hoge,@function
hoge:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	leal	12(%ebp), %eax
	movl	%eax, -4(%ebp)
	movl	%ebp, %esp
	popl	%ebp
	ret
.Lfe1:
	.size	 hoge,.Lfe1-hoge
		.section	.rodata
.LC0:
	.string	"hoge:%d\n"
.text
	.align 16
.globl main
	.type	 main,@function
main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	subl	$8, %esp
	pushl	$100
	pushl	$.LC0
	call	hoge
	addl	$16, %esp
	movl	$0, %eax
	movl	%ebp, %esp
	popl	%ebp
	ret
.Lfe2:
	.size	 main,.Lfe2-main
	.ident	"GCC: (GNU) 2.96 20000731 (Kondara MNU/Linux 2.0)"
