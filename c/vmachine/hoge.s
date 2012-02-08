	.file	"hoge.c"
	.version	"01.01"
gcc2_compiled.:
.section	.rodata
.LC0:
	.string	"RRRRRR %d\n"
.text
	.align 16
.globl ringofunc
	.type	 ringofunc,@function
ringofunc:
	pushl %ebp
	movl %esp,%ebp
	movl 8(%ebp),%eax
	pushl %eax
	pushl $.LC0
	call printf
	addl $8,%esp
.L1:
	movl %ebp,%esp
	popl %ebp
	ret
.Lfe1:
	.size	 ringofunc,.Lfe1-ringofunc
.section	.rodata
.LC1:
	.string	"aaaaa is called: %d\n"
.text
	.align 16
.globl aaaaa
	.type	 aaaaa,@function
aaaaa:
	pushl %ebp
	movl %esp,%ebp
	movl 8(%ebp),%eax
	pushl %eax
	pushl $.LC1
	call printf
	addl $8,%esp
.L2:
	movl %ebp,%esp
	popl %ebp
	ret
.Lfe2:
	.size	 aaaaa,.Lfe2-aaaaa
.section	.rodata
.LC2:
	.string	"%d\n"
.text
	.align 16
.globl main
	.type	 main,@function
main:
	pushl %ebp
	movl %esp,%ebp
	subl $4,%esp
	pushl %ebx
	pushl $ringofunc
	pushl $.LC2
	call printf
	addl $8,%esp
	pushl $aaaaa
	pushl $.LC2
	call printf
	addl $8,%esp
	movl $134514176,-4(%ebp)
	pushl $100
	movl -4(%ebp),%ebx
	call *%ebx
	addl $4,%esp
	movl $134514144,-4(%ebp)
	pushl $200
	movl -4(%ebp),%ebx
	call *%ebx
	addl $4,%esp
.L3:
	movl -8(%ebp),%ebx
	movl %ebp,%esp
	popl %ebp
	ret
.Lfe3:
	.size	 main,.Lfe3-main
	.ident	"GCC: (GNU) egcs-2.91.57 19980901 (egcs-1.1 release)"
