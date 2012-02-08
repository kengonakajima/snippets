	.file	"aho.c"
	.version	"01.01"
gcc2_compiled.:
.text
	.align 4
.globl aho
	.type	 aho,@function
aho:
	pushl	%ebp
	movl	$1000000, %eax
	movl	%esp, %ebp
	popl	%ebp
	ret
.Lfe1:
	.size	 aho,.Lfe1-aho
	.align 4
.globl main
	.type	 main,@function
main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$20, %esp
	pushl	$100
	call	aho
	movl	$200, (%esp)
	call	aho
	xorl	%eax, %eax
	leave
	ret
.Lfe2:
	.size	 main,.Lfe2-main
	.ident	"GCC: (GNU) 2.96 20000731 (Red Hat Linux 7.1 2.96-98)"
