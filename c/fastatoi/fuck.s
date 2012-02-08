	.file	"fuck.c"
	.version	"01.01"
gcc2_compiled.:
.text
	.align 16
.globl fuckfunc
	.type	 fuckfunc,@function
fuckfunc:
	pushl %ebp
	movl %esp,%ebp
	movl %ebp,%esp
	popl %ebp
	ret
.Lfe1:
	.size	 fuckfunc,.Lfe1-fuckfunc
	.ident	"GCC: (GNU) egcs-2.91.57 19980901 (egcs-1.1 release)"
