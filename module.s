	.text
	.file	"lightir"
	.globl	tigermain                       # -- Begin function tigermain
	.p2align	4, 0x90
	.type	tigermain,@function
tigermain:                              # @tigermain
	.cfi_startproc
# %bb.0:                                # %tigermain
	pushq	%rax
	.cfi_def_cfa_offset 16
	movq	%rdi, %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	subq	(%rcx), %rax
	movq	%rax, (%rsp)                    # 8-byte Spill
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	movq	(%rcx), %rdx
	addq	$-8, %rdx
	movq	%rax, %rcx
	addq	%rdx, %rcx
	movl	$5, (%rcx)
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movl	-8(%rax), %edi
	callq	printi@PLT
	movq	str.3@GOTPCREL(%rip), %rdi
	callq	print@PLT
	movq	(%rsp), %rsi                    # 8-byte Reload
	movl	$2, %edx
	movq	%rsi, %rdi
	callq	g@PLT
	movq	(%rsp), %rax                    # 8-byte Reload
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movl	-8(%rax), %edi
	callq	printi@PLT
	xorl	%eax, %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	tigermain, .Lfunc_end0-tigermain
	.cfi_endproc
                                        # -- End function
	.globl	g                               # -- Begin function g
	.p2align	4, 0x90
	.type	g,@function
g:                                      # @g
	.cfi_startproc
# %bb.0:                                # %g
	pushq	%rax
	.cfi_def_cfa_offset 16
	movq	%rdi, %rax
	movq	g_framesize.2@GOTPCREL(%rip), %rcx
	subq	(%rcx), %rax
	movq	%rax, (%rsp)                    # 8-byte Spill
	movq	g_framesize.2@GOTPCREL(%rip), %rdi
	movq	%rax, %rcx
	addq	(%rdi), %rcx
	movq	%rsi, 8(%rcx)
	movl	%edx, 16(%rcx)
	movq	g_framesize.2@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	cmpl	$3, 16(%rax)
	jle	.LBB1_2
# %bb.1:                                # %if_then2
	movq	str@GOTPCREL(%rip), %rdi
	callq	print@PLT
	jmp	.LBB1_3
.LBB1_2:                                # %if_else
	movq	(%rsp), %rax                    # 8-byte Reload
	movq	g_framesize.2@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movl	$4, -8(%rax)
.LBB1_3:                                # %if_next
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	g, .Lfunc_end1-g
	.cfi_endproc
                                        # -- End function
	.type	tigermain_framesize.1,@object   # @tigermain_framesize.1
	.section	.rodata,"a",@progbits
	.globl	tigermain_framesize.1
	.p2align	3
tigermain_framesize.1:
	.quad	32                              # 0x20
	.size	tigermain_framesize.1, 8

	.type	g_framesize.2,@object           # @g_framesize.2
	.globl	g_framesize.2
	.p2align	3
g_framesize.2:
	.quad	24                              # 0x18
	.size	g_framesize.2, 8

	.type	.L__unnamed_1,@object           # @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"hey! Bigger than 3!\n"
	.size	.L__unnamed_1, 21

	.type	str,@object                     # @str
	.section	.data.rel.ro,"aw",@progbits
	.globl	str
	.p2align	3
str:
	.long	20                              # 0x14
	.zero	4
	.quad	.L__unnamed_1
	.size	str, 16

	.type	.L__unnamed_2,@object           # @1
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_2:
	.asciz	"\n"
	.size	.L__unnamed_2, 2

	.type	str.3,@object                   # @str.3
	.section	.data.rel.ro,"aw",@progbits
	.globl	str.3
	.p2align	3
str.3:
	.long	1                               # 0x1
	.zero	4
	.quad	.L__unnamed_2
	.size	str.3, 16

	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym print
	.addrsig_sym printi
	.addrsig_sym g
	.addrsig_sym tigermain_framesize.1
	.addrsig_sym g_framesize.2
	.addrsig_sym str
	.addrsig_sym str.3
