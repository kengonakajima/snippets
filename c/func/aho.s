.data
.cstring
	.align 2
___func__.0:
	.ascii "hoge\0"
	.align 2
LC0:
	.ascii "%s\12\0"
.text
	.align 2
	.globl _hoge
_hoge:
	mflr r0
	stmw r30,-8(r1)
	stw r0,8(r1)
	stwu r1,-80(r1)
	mr r30,r1
	bcl 20,31,L1$pb
L1$pb:
	mflr r31
	addis r3,r31,ha16(LC0-L1$pb)
	la r3,lo16(LC0-L1$pb)(r3)
	addis r4,r31,ha16(___func__.0-L1$pb)
	la r4,lo16(___func__.0-L1$pb)(r4)
	bl L_printf$stub
	lwz r1,0(r1)
	lwz r0,8(r1)
	mtlr r0
	lmw r30,-8(r1)
	blr
.data
.cstring
	.align 2
___func__.1:
	.ascii "main\0"
.text
	.align 2
	.globl _main
_main:
	mflr r0
	stmw r30,-8(r1)
	stw r0,8(r1)
	stwu r1,-80(r1)
	mr r30,r1
	bcl 20,31,L2$pb
L2$pb:
	mflr r31
	bl _hoge
	addis r3,r31,ha16(LC0-L2$pb)
	la r3,lo16(LC0-L2$pb)(r3)
	addis r4,r31,ha16(___func__.1-L2$pb)
	la r4,lo16(___func__.1-L2$pb)(r4)
	bl L_printf$stub
	mr r3,r0
	lwz r1,0(r1)
	lwz r0,8(r1)
	mtlr r0
	lmw r30,-8(r1)
	blr
.data
.picsymbol_stub
L_printf$stub:
	.indirect_symbol _printf
	mflr r0
	bcl 20,31,L0$_printf
L0$_printf:
	mflr r11
	addis r11,r11,ha16(L_printf$lazy_ptr-L0$_printf)
	mtlr r0
	lwz r12,lo16(L_printf$lazy_ptr-L0$_printf)(r11)
	mtctr r12
	addi r11,r11,lo16(L_printf$lazy_ptr-L0$_printf)
	bctr
.data
.lazy_symbol_pointer
L_printf$lazy_ptr:
	.indirect_symbol _printf
	.long dyld_stub_binding_helper
