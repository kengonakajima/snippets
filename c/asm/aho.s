	.file	"aho.c"
	.version	"01.01"
gcc2_compiled.:
.text
	.align 16
.globl main
	.type	 main,@function
main:
	pushl %ebp
	movl %esp,%ebp
	subl $4,%esp
	movl $100,-4(%ebp)
.L1:
	movl %ebp,%esp
	popl %ebp
	ret
.Lfe1:
	.size	 main,.Lfe1-main
	.ident	"GCC: (GNU) egcs-2.91.57 19980901 (egcs-1.1 release)"
