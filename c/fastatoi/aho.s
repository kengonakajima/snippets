	.file	"aho.c"
	.version	"01.01"
gcc2_compiled.:
.section	.rodata
.LC0:
	.string	"asdfa\n"
.LC1:
	.string	"7777"
.LC2:
	.string	"%d\n"
.text
	.align 16
.globl main
	.type	 main,@function
main:
	pushl %ebp
	movl %esp,%ebp
	subl $44,%esp
	pushl %edi
	movl .LC1,%eax
	pushl %esi
	leal -16(%ebp),%edi
	pushl %ebx
	movl %eax,-8(%ebp)
	movb .LC1+4,%al
	movb %al,-4(%ebp)
	pushl $0
	movl %edi,-36(%ebp)
	pushl %edi
	call gettimeofday
	leal -8(%ebp),%edi
	movl %edi,-40(%ebp)
.L180:
	movl (%edi),%ebx
	testb %bl,%bl
	je .L183
	testb %bh,%bh
	je .L182
	testl $16711680,%ebx
	je .L181
	addl $4,%edi
	testl $-16777216,%ebx
	jne .L180
	subl $3,%edi
.L181:
	incl %edi
.L182:
	incl %edi
.L183:
	movl %edi,-32(%ebp)
	movl -40(%ebp),%edi
	subl %edi,-32(%ebp)
	movl $0,-28(%ebp)
	addl $8,%esp
	.p2align 4,,7
.L161:
	movl -40(%ebp),%esi
	movl -32(%ebp),%eax
	movl $1,-44(%ebp)
	cmpb $45,-8(%ebp)
	jne .L162
	movl $-1,-44(%ebp)
	decl %eax
	leal -7(%ebp),%esi
.L162:
	cmpl $10,%eax
	ja .L163
	jmp *.L164(,%eax,4)
	.p2align 4,,7
.section	.rodata
	.align 4
	.align 4
.L164:
	.long .L165
	.long .L166
	.long .L167
	.long .L168
	.long .L169
	.long .L170
	.long .L171
	.long .L172
	.long .L173
	.long .L174
	.long .L175
.text
	.p2align 4,,7
.L165:
	xorl %eax,%eax
	jmp .L176
	.p2align 4,,7
.L166:
	movsbl (%esi),%eax
	addl $-48,%eax
	imull -44(%ebp),%eax
	jmp .L176
	.p2align 4,,7
.L167:
	movsbl (%esi),%eax
	leal -240(%eax,%eax,4),%eax
	movsbl 1(%esi),%edx
	leal -48(%edx,%eax,2),%eax
	imull -44(%ebp),%eax
	jmp .L176
	.p2align 4,,7
.L168:
	movsbl (%esi),%eax
	movsbl 1(%esi),%edx
	leal -240(%eax,%eax,4),%eax
	leal -240(%edx,%edx,4),%edx
	addl %edx,%edx
	leal (%eax,%eax,4),%eax
	leal -48(%edx,%eax,4),%eax
	movsbl 2(%esi),%edx
	jmp .L179
	.p2align 4,,7
.L169:
	movsbl (%esi),%eax
	addl $-48,%eax
	movl %eax,%edx
	sall $5,%edx
	subl %eax,%edx
	leal (%eax,%edx,4),%edx
	movsbl 1(%esi),%eax
	leal -240(%eax,%eax,4),%eax
	leal (%eax,%eax,4),%eax
	sall $2,%eax
	leal (%eax,%edx,8),%edx
	movsbl 2(%esi),%eax
	leal -240(%eax,%eax,4),%eax
	leal -48(%edx,%eax,2),%eax
	movsbl 3(%esi),%edx
	jmp .L179
	.p2align 4,,7
.L170:
	movsbl (%esi),%edx
	addl $-48,%edx
	movl %edx,%eax
	sall $5,%eax
	subl %edx,%eax
	movsbl 1(%esi),%ecx
	addl $-48,%ecx
	leal (%edx,%eax,4),%eax
	movl %ecx,%edx
	sall $5,%edx
	leal (%eax,%eax,4),%eax
	subl %ecx,%edx
	sall $4,%eax
	leal (%ecx,%edx,4),%edx
	leal (%eax,%edx,8),%edx
	movsbl 2(%esi),%eax
	leal -240(%eax,%eax,4),%eax
	leal (%eax,%eax,4),%eax
	leal (%edx,%eax,4),%eax
	movsbl 3(%esi),%edx
	leal -240(%edx,%edx,4),%edx
	leal -48(%eax,%edx,2),%edx
	movsbl 4(%esi),%eax
	jmp .L179
	.p2align 4,,7
.L171:
	movsbl (%esi),%eax
	addl $-48,%eax
	movl %eax,%ecx
	sall $5,%ecx
	subl %eax,%ecx
	movsbl 1(%esi),%edx
	addl $-48,%edx
	leal (%eax,%ecx,4),%ecx
	movl %edx,%eax
	sall $5,%eax
	subl %edx,%eax
	leal (%edx,%eax,4),%eax
	leal (%ecx,%ecx,4),%ecx
	leal (%ecx,%ecx,4),%ecx
	leal (%eax,%eax,4),%eax
	sall $4,%eax
	sall $5,%ecx
	addl %eax,%ecx
	movsbl 2(%esi),%eax
	addl $-48,%eax
	movl %eax,%edx
	sall $5,%edx
	subl %eax,%edx
	leal (%eax,%edx,4),%edx
	movsbl 3(%esi),%eax
	leal -240(%eax,%eax,4),%eax
	leal (%ecx,%edx,8),%edx
	leal (%eax,%eax,4),%eax
	leal (%edx,%eax,4),%eax
	movsbl 4(%esi),%edx
	leal -240(%edx,%edx,4),%edx
	leal -48(%eax,%edx,2),%edx
	movsbl 5(%esi),%eax
	jmp .L179
	.p2align 4,,7
.L172:
	movsbl (%esi),%edx
	addl $-48,%edx
	movl %edx,%eax
	sall $5,%eax
	subl %edx,%eax
	movl %eax,%ecx
	sall $6,%ecx
	subl %eax,%ecx
	leal (%edx,%ecx,8),%ecx
	movsbl 1(%esi),%edx
	addl $-48,%edx
	movl %edx,%eax
	sall $5,%eax
	subl %edx,%eax
	leal (%edx,%eax,4),%eax
	leal (%eax,%eax,4),%eax
	leal (%eax,%eax,4),%eax
	sall $5,%eax
	sall $6,%ecx
	movsbl 2(%esi),%edx
	addl $-48,%edx
	addl %eax,%ecx
	movl %edx,%eax
	sall $5,%eax
	subl %edx,%eax
	leal (%edx,%eax,4),%eax
	leal (%eax,%eax,4),%eax
	sall $4,%eax
	addl %eax,%ecx
	movsbl 3(%esi),%eax
	addl $-48,%eax
	movl %eax,%edx
	sall $5,%edx
	subl %eax,%edx
	leal (%eax,%edx,4),%edx
	movsbl 4(%esi),%eax
	leal -240(%eax,%eax,4),%eax
	leal (%ecx,%edx,8),%edx
	leal (%eax,%eax,4),%eax
	leal (%edx,%eax,4),%eax
	movsbl 5(%esi),%edx
	leal -240(%edx,%edx,4),%edx
	leal -48(%eax,%edx,2),%edx
	movsbl 6(%esi),%eax
	jmp .L179
	.p2align 4,,7
.L173:
	movsbl (%esi),%edx
	addl $-48,%edx
	movl %edx,%eax
	sall $5,%eax
	subl %edx,%eax
	movl %eax,%ecx
	sall $6,%ecx
	subl %eax,%ecx
	movsbl 1(%esi),%ebx
	addl $-48,%ebx
	leal (%edx,%ecx,8),%ecx
	movl %ebx,%edx
	sall $5,%edx
	subl %ebx,%edx
	movl %edx,%eax
	sall $6,%eax
	subl %edx,%eax
	leal (%ecx,%ecx,4),%ecx
	leal (%ebx,%eax,8),%eax
	sall $6,%eax
	sall $7,%ecx
	movsbl 2(%esi),%edx
	addl $-48,%edx
	addl %eax,%ecx
	movl %edx,%eax
	sall $5,%eax
	subl %edx,%eax
	leal (%edx,%eax,4),%eax
	leal (%eax,%eax,4),%eax
	leal (%eax,%eax,4),%eax
	sall $5,%eax
	movsbl 3(%esi),%edx
	addl $-48,%edx
	addl %eax,%ecx
	movl %edx,%eax
	sall $5,%eax
	subl %edx,%eax
	leal (%edx,%eax,4),%eax
	leal (%eax,%eax,4),%eax
	sall $4,%eax
	addl %eax,%ecx
	movsbl 4(%esi),%eax
	addl $-48,%eax
	movl %eax,%edx
	sall $5,%edx
	subl %eax,%edx
	leal (%eax,%edx,4),%edx
	movsbl 5(%esi),%eax
	leal -240(%eax,%eax,4),%eax
	leal (%ecx,%edx,8),%edx
	leal (%eax,%eax,4),%eax
	leal (%edx,%eax,4),%eax
	movsbl 6(%esi),%edx
	leal -240(%edx,%edx,4),%edx
	leal -48(%eax,%edx,2),%edx
	movsbl 7(%esi),%eax
	jmp .L179
	.p2align 4,,7
.L174:
	movsbl (%esi),%edx
	addl $-48,%edx
	movl %edx,%eax
	sall $5,%eax
	subl %edx,%eax
	movl %eax,%ebx
	sall $6,%ebx
	subl %eax,%ebx
	movsbl 1(%esi),%ecx
	addl $-48,%ecx
	leal (%edx,%ebx,8),%ebx
	movl %ecx,%edx
	sall $5,%edx
	subl %ecx,%edx
	movl %edx,%eax
	sall $6,%eax
	subl %edx,%eax
	leal (%ecx,%eax,8),%eax
	movsbl 2(%esi),%ecx
	addl $-48,%ecx
	leal (%ebx,%ebx,4),%ebx
	leal (%ebx,%ebx,4),%ebx
	leal (%eax,%eax,4),%eax
	movl %ecx,%edx
	sall $7,%eax
	sall $8,%ebx
	sall $5,%edx
	subl %ecx,%edx
	addl %eax,%ebx
	movl %edx,%eax
	sall $6,%eax
	subl %edx,%eax
	leal (%ecx,%eax,8),%eax
	sall $6,%eax
	movsbl 3(%esi),%edx
	addl $-48,%edx
	addl %eax,%ebx
	movl %edx,%eax
	sall $5,%eax
	subl %edx,%eax
	leal (%edx,%eax,4),%eax
	leal (%eax,%eax,4),%eax
	leal (%eax,%eax,4),%eax
	sall $5,%eax
	movsbl 4(%esi),%edx
	addl $-48,%edx
	addl %eax,%ebx
	movl %edx,%eax
	sall $5,%eax
	subl %edx,%eax
	leal (%edx,%eax,4),%eax
	leal (%eax,%eax,4),%eax
	sall $4,%eax
	addl %eax,%ebx
	movsbl 5(%esi),%eax
	addl $-48,%eax
	movl %eax,%edx
	sall $5,%edx
	subl %eax,%edx
	leal (%eax,%edx,4),%edx
	movsbl 6(%esi),%eax
	leal -240(%eax,%eax,4),%eax
	leal (%ebx,%edx,8),%edx
	leal (%eax,%eax,4),%eax
	leal (%edx,%eax,4),%eax
	movsbl 7(%esi),%edx
	leal -240(%edx,%edx,4),%edx
	leal -48(%eax,%edx,2),%edx
	movsbl 8(%esi),%eax
	jmp .L179
	.p2align 4,,7
.L175:
	movsbl 1(%esi),%ecx
	addl $-48,%ecx
	movl %ecx,%edx
	sall $5,%edx
	subl %ecx,%edx
	movl %edx,%eax
	sall $6,%eax
	subl %edx,%eax
	leal (%ecx,%eax,8),%eax
	movsbl 2(%esi),%ecx
	addl $-48,%ecx
	leal (%eax,%eax,4),%eax
	movl %ecx,%edx
	movsbl (%esi),%ebx
	leal (%eax,%eax,4),%eax
	sall $8,%eax
	imull $1000000000,%ebx,%ebx
	sall $5,%edx
	subl %ecx,%edx
	leal -755359744(%eax,%ebx),%ebx
	movl %edx,%eax
	sall $6,%eax
	subl %edx,%eax
	leal (%ecx,%eax,8),%eax
	movsbl 3(%esi),%ecx
	addl $-48,%ecx
	leal (%eax,%eax,4),%eax
	movl %ecx,%edx
	sall $7,%eax
	sall $5,%edx
	subl %ecx,%edx
	addl %eax,%ebx
	movl %edx,%eax
	sall $6,%eax
	subl %edx,%eax
	leal (%ecx,%eax,8),%eax
	sall $6,%eax
	movsbl 4(%esi),%edx
	addl $-48,%edx
	addl %eax,%ebx
	movl %edx,%eax
	sall $5,%eax
	subl %edx,%eax
	leal (%edx,%eax,4),%eax
	leal (%eax,%eax,4),%eax
	leal (%eax,%eax,4),%eax
	sall $5,%eax
	movsbl 5(%esi),%edx
	addl $-48,%edx
	addl %eax,%ebx
	movl %edx,%eax
	sall $5,%eax
	subl %edx,%eax
	leal (%edx,%eax,4),%eax
	leal (%eax,%eax,4),%eax
	sall $4,%eax
	addl %eax,%ebx
	movsbl 6(%esi),%eax
	addl $-48,%eax
	movl %eax,%edx
	sall $5,%edx
	subl %eax,%edx
	leal (%eax,%edx,4),%edx
	movsbl 7(%esi),%eax
	leal -240(%eax,%eax,4),%eax
	leal (%ebx,%edx,8),%edx
	leal (%eax,%eax,4),%eax
	leal (%edx,%eax,4),%eax
	movsbl 8(%esi),%edx
	leal -240(%edx,%edx,4),%edx
	leal -48(%eax,%edx,2),%edx
	movsbl 9(%esi),%eax
.L179:
	addl %edx,%eax
	imull -44(%ebp),%eax
	jmp .L176
	.p2align 4,,7
.L163:
	pushl $.LC0
	call printf
	xorl %eax,%eax
	addl $4,%esp
.L176:
	pushl %eax
	call fuckfunc
	addl $4,%esp
	incl -28(%ebp)
	cmpl $9999999,-28(%ebp)
	jle .L161
	pushl $0
	leal -24(%ebp),%ebx
	pushl %ebx
	call gettimeofday
	movl -16(%ebp),%eax
	movl -24(%ebp),%ecx
	subl %eax,%ecx
	movl %ecx,%edx
	sall $5,%edx
	subl %ecx,%edx
	movl %edx,%eax
	movl -36(%ebp),%edi
	sall $6,%eax
	subl %edx,%eax
	movl 4(%edi),%edx
	movl 4(%ebx),%ebx
	subl %edx,%ebx
	leal (%ecx,%eax,8),%eax
	movl %ebx,%edx
	sall $6,%eax
	addl %edx,%eax
	pushl %eax
	pushl $.LC2
	call printf
	leal -56(%ebp),%esp
	xorl %eax,%eax
	popl %ebx
	popl %esi
	popl %edi
	movl %ebp,%esp
	popl %ebp
	ret
.Lfe1:
	.size	 main,.Lfe1-main
	.ident	"GCC: (GNU) egcs-2.91.57 19980901 (egcs-1.1 release)"