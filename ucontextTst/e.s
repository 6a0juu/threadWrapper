	.file	"e.c"
	.def	__main;	.scl	2;	.type	32;	.endef
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	call	__main
	movl	$0, %ecx
	call	exit
	nop
	.seh_endproc
	.ident	"GCC: (Rev2, Built by MSYS2 project) 7.2.0"
	.def	exit;	.scl	2;	.type	32;	.endef
