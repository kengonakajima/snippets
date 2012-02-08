	.file	"aho.c"
	.version	"01.01"
gcc2_compiled.:
.text
	.align 16
.globl main
	.type	 main,@function
main:
	pushl	%ebp
	movl	$-1717986918, %edx
	movl	%esp, %ebp
	pushl	%ebx
	subl	$20, %esp
	movl	$1076140441, %ecx
	xorl	%ebx, %ebx
	.p2align 4,,7
.L143:
	movl	$19, %eax
	.p2align 4,,7
.L147:
	movl	%edx, -24(%ebp)
	movl	%ecx, -20(%ebp)
	fldl	-24(%ebp)
#APP
	fsqrt
#NO_APP
	fstp	%st(0)
	decl	%eax
	jns	.L147
	incl	%ebx
	cmpl	$29999, %ebx
	jle	.L143
	addl	$20, %esp
	xorl	%eax, %eax
	popl	%ebx
	popl	%ebp
	ret
.Lfe1:
	.size	 main,.Lfe1-main
	.ident	"GCC: (GNU) 2.96 20000731 (Kondara MNU/Linux 2.0)"
