	.section __TEXT,__textcoal_nt,coalesced,pure_instructions
	.align 1
.globl __ZN3ahoC1Ei
	.weak_definition __ZN3ahoC1Ei
__ZN3ahoC1Ei:
LFB5:
	pushl	%ebp
LCFI0:
	movl	%esp, %ebp
LCFI1:
	subl	$8, %esp
LCFI2:
	movl	8(%ebp), %edx
	movl	12(%ebp), %eax
	movl	%eax, (%edx)
	leave
	ret
LFE5:
	.cstring
LC0:
	.ascii "%d\12\0"
	.section __TEXT,__textcoal_nt,coalesced,pure_instructions
	.align 1
.globl __ZN3aho1pEv
	.weak_definition __ZN3aho1pEv
__ZN3aho1pEv:
LFB6:
	pushl	%ebp
LCFI3:
	movl	%esp, %ebp
LCFI4:
	pushl	%ebx
LCFI5:
	subl	$20, %esp
LCFI6:
	call	___i686.get_pc_thunk.bx
"L00000000001$pb":
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, 4(%esp)
	leal	LC0-"L00000000001$pb"(%ebx), %eax
	movl	%eax, (%esp)
	call	L_printf$stub
	addl	$20, %esp
	popl	%ebx
	popl	%ebp
	ret
LFE6:
	.text
	.align 1,0x90
.globl _main
_main:
LFB7:
	pushl	%ebp
LCFI7:
	movl	%esp, %ebp
LCFI8:
	pushl	%esi
LCFI9:
	subl	$36, %esp
LCFI10:
	movl	$4, (%esp)
	call	L__Znwm$stub
	movl	%eax, %esi
	movl	$10, 4(%esp)
	movl	%esi, (%esp)
	call	L__ZN3ahoC1Ei$stub
	movl	%esi, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	L__ZN3aho1pEv$stub
	movl	$0, %eax
	addl	$36, %esp
	popl	%esi
	popl	%ebp
	ret
LFE7:
	.section __TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame1:
	.set L$set$0,LECIE1-LSCIE1
	.long L$set$0
LSCIE1:
	.long	0x0
	.byte	0x1
	.ascii "zPR\0"
	.byte	0x1
	.byte	0x7c
	.byte	0x8
	.byte	0x6
	.byte	0x9b
	.long	L___gxx_personality_v0$non_lazy_ptr-.
	.byte	0x10
	.byte	0xc
	.byte	0x5
	.byte	0x4
	.byte	0x88
	.byte	0x1
	.align 2
LECIE1:
	.globl __ZN3ahoC1Ei.eh
	.weak_definition __ZN3ahoC1Ei.eh
__ZN3ahoC1Ei.eh:
LSFDE1:
	.set L$set$1,LEFDE1-LASFDE1
	.long L$set$1
LASFDE1:
	.long	LASFDE1-EH_frame1
	.long	LFB5-.
	.set L$set$2,LFE5-LFB5
	.long L$set$2
	.byte	0x0
	.byte	0x4
	.set L$set$3,LCFI0-LFB5
	.long L$set$3
	.byte	0xe
	.byte	0x8
	.byte	0x84
	.byte	0x2
	.byte	0x4
	.set L$set$4,LCFI1-LCFI0
	.long L$set$4
	.byte	0xd
	.byte	0x4
	.align 2
LEFDE1:
	.globl __ZN3aho1pEv.eh
	.weak_definition __ZN3aho1pEv.eh
__ZN3aho1pEv.eh:
LSFDE3:
	.set L$set$5,LEFDE3-LASFDE3
	.long L$set$5
LASFDE3:
	.long	LASFDE3-EH_frame1
	.long	LFB6-.
	.set L$set$6,LFE6-LFB6
	.long L$set$6
	.byte	0x0
	.byte	0x4
	.set L$set$7,LCFI3-LFB6
	.long L$set$7
	.byte	0xe
	.byte	0x8
	.byte	0x84
	.byte	0x2
	.byte	0x4
	.set L$set$8,LCFI4-LCFI3
	.long L$set$8
	.byte	0xd
	.byte	0x4
	.byte	0x4
	.set L$set$9,LCFI6-LCFI4
	.long L$set$9
	.byte	0x83
	.byte	0x3
	.align 2
LEFDE3:
	.globl _main.eh
_main.eh:
LSFDE5:
	.set L$set$10,LEFDE5-LASFDE5
	.long L$set$10
LASFDE5:
	.long	LASFDE5-EH_frame1
	.long	LFB7-.
	.set L$set$11,LFE7-LFB7
	.long L$set$11
	.byte	0x0
	.byte	0x4
	.set L$set$12,LCFI7-LFB7
	.long L$set$12
	.byte	0xe
	.byte	0x8
	.byte	0x84
	.byte	0x2
	.byte	0x4
	.set L$set$13,LCFI8-LCFI7
	.long L$set$13
	.byte	0xd
	.byte	0x4
	.byte	0x4
	.set L$set$14,LCFI10-LCFI8
	.long L$set$14
	.byte	0x86
	.byte	0x3
	.align 2
LEFDE5:
	.section __IMPORT,__jump_table,symbol_stubs,self_modifying_code+pure_instructions,5
L_printf$stub:
	.indirect_symbol _printf
	hlt ; hlt ; hlt ; hlt ; hlt
L__ZN3aho1pEv$stub:
	.indirect_symbol __ZN3aho1pEv
	hlt ; hlt ; hlt ; hlt ; hlt
	.section __IMPORT,__pointers,non_lazy_symbol_pointers
L___gxx_personality_v0$non_lazy_ptr:
	.indirect_symbol ___gxx_personality_v0
	.long	0
	.section __IMPORT,__jump_table,symbol_stubs,self_modifying_code+pure_instructions,5
L__Znwm$stub:
	.indirect_symbol __Znwm
	hlt ; hlt ; hlt ; hlt ; hlt
L__ZN3ahoC1Ei$stub:
	.indirect_symbol __ZN3ahoC1Ei
	hlt ; hlt ; hlt ; hlt ; hlt
	.constructor
	.destructor
	.align 1
	.subsections_via_symbols
	.section __TEXT,__textcoal_nt,coalesced,pure_instructions
.weak_definition	___i686.get_pc_thunk.bx
.private_extern	___i686.get_pc_thunk.bx
___i686.get_pc_thunk.bx:
	movl	(%esp), %ebx
	ret
