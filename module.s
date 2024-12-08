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
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	movq	%rax, %rcx
	addq	(%rdx), %rcx
	movl	$8, -8(%rcx)
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movl	-8(%rax), %edi
	xorl	%eax, %eax
	movl	%eax, %esi
	callq	init_array@PLT
	movq	%rax, %rdx
	movq	(%rsp), %rax                    # 8-byte Reload
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rsi
	movq	%rax, %rcx
	addq	(%rsi), %rcx
	movq	%rdx, -16(%rcx)
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movl	-8(%rax), %edi
	xorl	%eax, %eax
	movl	%eax, %esi
	callq	init_array@PLT
	movq	%rax, %rdx
	movq	(%rsp), %rax                    # 8-byte Reload
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rsi
	movq	%rax, %rcx
	addq	(%rsi), %rcx
	movq	%rdx, -24(%rcx)
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	movq	%rax, %rcx
	addq	(%rdx), %rcx
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rax
	movl	-8(%rcx), %edi
	addl	-8(%rax), %edi
	subl	$1, %edi
	xorl	%eax, %eax
	movl	%eax, %esi
	callq	init_array@PLT
	movq	%rax, %rdx
	movq	(%rsp), %rax                    # 8-byte Reload
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rsi
	movq	%rax, %rcx
	addq	(%rsi), %rcx
	movq	%rdx, -32(%rcx)
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	movq	%rax, %rcx
	addq	(%rdx), %rcx
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rax
	movl	-8(%rcx), %edi
	addl	-8(%rax), %edi
	subl	$1, %edi
	xorl	%eax, %eax
	movl	%eax, %esi
	callq	init_array@PLT
	movq	(%rsp), %rsi                    # 8-byte Reload
	movq	%rax, %rcx
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	movq	%rsi, %rax
	addq	(%rdx), %rax
	movq	%rcx, -40(%rax)
	xorl	%edx, %edx
	movq	%rsi, %rdi
	callq	try@PLT
	xorl	%eax, %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	tigermain, .Lfunc_end0-tigermain
	.cfi_endproc
                                        # -- End function
	.globl	printboard                      # -- Begin function printboard
	.p2align	4, 0x90
	.type	printboard,@function
printboard:                             # @printboard
	.cfi_startproc
# %bb.0:                                # %printboard
	subq	$56, %rsp
	.cfi_def_cfa_offset 64
	movq	%rdi, %rax
	movq	printboard_framesize.2@GOTPCREL(%rip), %rcx
	subq	(%rcx), %rax
	movq	%rax, 32(%rsp)                  # 8-byte Spill
	movq	printboard_framesize.2@GOTPCREL(%rip), %rdx
	movq	%rax, %rcx
	addq	(%rdx), %rcx
	movq	%rsi, 8(%rcx)
	movq	printboard_framesize.2@GOTPCREL(%rip), %rdx
	movq	%rax, %rcx
	addq	(%rdx), %rcx
	movq	8(%rcx), %rcx
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rcx
	movl	-8(%rcx), %ecx
	subl	$1, %ecx
	movl	%ecx, 44(%rsp)                  # 4-byte Spill
	movq	printboard_framesize.2@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	%rax, %rcx
	addq	$-8, %rcx
	movq	%rcx, 48(%rsp)                  # 8-byte Spill
	movl	$0, -8(%rax)
	jmp	.LBB1_2
.LBB1_1:                                # %for_incre
                                        #   in Loop: Header=BB1_2 Depth=1
	movl	44(%rsp), %ecx                  # 4-byte Reload
	movq	48(%rsp), %rdx                  # 8-byte Reload
	movl	(%rdx), %eax
	addl	$1, %eax
	movl	%eax, (%rdx)
	cmpl	%ecx, %eax
	jg	.LBB1_9
.LBB1_2:                                # %for_body
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB1_4 Depth 2
	movq	32(%rsp), %rax                  # 8-byte Reload
	movq	printboard_framesize.2@GOTPCREL(%rip), %rdx
	movq	%rax, %rcx
	addq	(%rdx), %rcx
	movq	8(%rcx), %rcx
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rcx
	movl	-8(%rcx), %ecx
	subl	$1, %ecx
	movl	%ecx, 20(%rsp)                  # 4-byte Spill
	movq	printboard_framesize.2@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	%rax, %rcx
	addq	$-16, %rcx
	movq	%rcx, 24(%rsp)                  # 8-byte Spill
	movl	$0, -16(%rax)
	jmp	.LBB1_4
.LBB1_3:                                # %for_incre2
                                        #   in Loop: Header=BB1_4 Depth=2
	movl	20(%rsp), %ecx                  # 4-byte Reload
	movq	24(%rsp), %rdx                  # 8-byte Reload
	movl	(%rdx), %eax
	addl	$1, %eax
	movl	%eax, (%rdx)
	cmpl	%ecx, %eax
	jg	.LBB1_8
.LBB1_4:                                # %for_body3
                                        #   Parent Loop BB1_2 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movq	32(%rsp), %rcx                  # 8-byte Reload
	movq	printboard_framesize.2@GOTPCREL(%rip), %rdx
	movq	%rcx, %rax
	addq	(%rdx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rax
	movq	printboard_framesize.2@GOTPCREL(%rip), %rsi
	movq	%rcx, %rdx
	addq	(%rsi), %rdx
	movl	-8(%rdx), %edx
	movq	-24(%rax), %rax
	movq	printboard_framesize.2@GOTPCREL(%rip), %rsi
	addq	(%rsi), %rcx
	movslq	%edx, %rdx
	movl	(%rax,%rdx,4), %eax
	cmpl	-16(%rcx), %eax
	jne	.LBB1_6
# %bb.5:                                # %if_then4
                                        #   in Loop: Header=BB1_4 Depth=2
	movq	str@GOTPCREL(%rip), %rax
	movq	%rax, 8(%rsp)                   # 8-byte Spill
	jmp	.LBB1_7
.LBB1_6:                                # %if_else
                                        #   in Loop: Header=BB1_4 Depth=2
	movq	str.3@GOTPCREL(%rip), %rax
	movq	%rax, 8(%rsp)                   # 8-byte Spill
	jmp	.LBB1_7
.LBB1_7:                                # %if_next
                                        #   in Loop: Header=BB1_4 Depth=2
	movq	8(%rsp), %rdi                   # 8-byte Reload
	callq	print@PLT
	jmp	.LBB1_3
.LBB1_8:                                # %for_exit
                                        #   in Loop: Header=BB1_2 Depth=1
	movq	str.4@GOTPCREL(%rip), %rdi
	callq	print@PLT
	jmp	.LBB1_1
.LBB1_9:                                # %for_exit5
	movq	str.5@GOTPCREL(%rip), %rdi
	callq	print@PLT
	addq	$56, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	printboard, .Lfunc_end1-printboard
	.cfi_endproc
                                        # -- End function
	.globl	try                             # -- Begin function try
	.p2align	4, 0x90
	.type	try,@function
try:                                    # @try
	.cfi_startproc
# %bb.0:                                # %try
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movq	%rdi, %rcx
	movq	try_framesize.6@GOTPCREL(%rip), %rax
	subq	(%rax), %rcx
	movq	%rcx, 16(%rsp)                  # 8-byte Spill
	movq	try_framesize.6@GOTPCREL(%rip), %rdi
	movq	%rcx, %rax
	addq	(%rdi), %rax
	movq	%rsi, 8(%rax)
	movl	%edx, 16(%rax)
	movq	try_framesize.6@GOTPCREL(%rip), %rdx
	movq	%rcx, %rax
	addq	(%rdx), %rax
	movq	try_framesize.6@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rcx
	movq	8(%rcx), %rcx
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rcx
	movl	16(%rax), %eax
	cmpl	-8(%rcx), %eax
	jne	.LBB2_2
# %bb.1:                                # %if_then2
	movq	16(%rsp), %rdi                  # 8-byte Reload
	movq	try_framesize.6@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rsi
	callq	printboard@PLT
	jmp	.LBB2_8
.LBB2_2:                                # %if_else
	movq	16(%rsp), %rax                  # 8-byte Reload
	movq	try_framesize.6@GOTPCREL(%rip), %rdx
	movq	%rax, %rcx
	addq	(%rdx), %rcx
	movq	8(%rcx), %rcx
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rcx
	movl	-8(%rcx), %ecx
	subl	$1, %ecx
	movl	%ecx, 4(%rsp)                   # 4-byte Spill
	movq	try_framesize.6@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	%rax, %rcx
	addq	$-8, %rcx
	movq	%rcx, 8(%rsp)                   # 8-byte Spill
	movl	$0, -8(%rax)
	jmp	.LBB2_4
.LBB2_3:                                # %for_incre
                                        #   in Loop: Header=BB2_4 Depth=1
	movl	4(%rsp), %ecx                   # 4-byte Reload
	movq	8(%rsp), %rdx                   # 8-byte Reload
	movl	(%rdx), %eax
	addl	$1, %eax
	movl	%eax, (%rdx)
	cmpl	%ecx, %eax
	jg	.LBB2_7
.LBB2_4:                                # %for_body
                                        # =>This Inner Loop Header: Depth=1
	movq	16(%rsp), %rsi                  # 8-byte Reload
	movq	try_framesize.6@GOTPCREL(%rip), %rcx
	movq	%rsi, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	try_framesize.6@GOTPCREL(%rip), %rdx
	movq	%rsi, %rcx
	addq	(%rdx), %rcx
	movl	-8(%rcx), %ecx
	movq	-16(%rax), %rax
	movslq	%ecx, %rcx
	cmpl	$0, (%rax,%rcx,4)
	sete	%al
	movq	try_framesize.6@GOTPCREL(%rip), %rdx
	movq	%rsi, %rcx
	addq	(%rdx), %rcx
	movq	8(%rcx), %rcx
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rcx
	movq	try_framesize.6@GOTPCREL(%rip), %rdi
	movq	%rsi, %rdx
	addq	(%rdi), %rdx
	movq	try_framesize.6@GOTPCREL(%rip), %r8
	movq	%rsi, %rdi
	addq	(%r8), %rdi
	movl	-8(%rdx), %edx
	addl	16(%rdi), %edx
	movl	%edx, %edx
                                        # kill: def $rdx killed $edx
	movl	(%rdx), %edx
	movq	-32(%rcx), %rcx
	movslq	%edx, %rdx
	cmpl	$0, (%rcx,%rdx,4)
	sete	%cl
	andb	%cl, %al
	movq	try_framesize.6@GOTPCREL(%rip), %rdx
	movq	%rsi, %rcx
	addq	(%rdx), %rcx
	movq	8(%rcx), %rcx
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rcx
	movq	try_framesize.6@GOTPCREL(%rip), %rdi
	movq	%rsi, %rdx
	addq	(%rdi), %rdx
	movl	-8(%rdx), %edx
	addl	$7, %edx
	movq	try_framesize.6@GOTPCREL(%rip), %rdi
	addq	(%rdi), %rsi
	subl	16(%rsi), %edx
	movl	%edx, %edx
                                        # kill: def $rdx killed $edx
	movl	(%rdx), %edx
	movq	-40(%rcx), %rcx
	movslq	%edx, %rdx
	cmpl	$0, (%rcx,%rdx,4)
	sete	%cl
	andb	%cl, %al
	testb	$1, %al
	jne	.LBB2_5
	jmp	.LBB2_6
.LBB2_5:                                # %if_then3
                                        #   in Loop: Header=BB2_4 Depth=1
	movq	16(%rsp), %rdi                  # 8-byte Reload
	movq	try_framesize.6@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	try_framesize.6@GOTPCREL(%rip), %rdx
	movq	%rdi, %rcx
	addq	(%rdx), %rcx
	movl	-8(%rcx), %ecx
	movq	-16(%rax), %rax
	movslq	%ecx, %rcx
	movl	$1, (%rax,%rcx,4)
	movq	try_framesize.6@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	try_framesize.6@GOTPCREL(%rip), %rdx
	movq	%rdi, %rcx
	addq	(%rdx), %rcx
	movq	try_framesize.6@GOTPCREL(%rip), %rsi
	movq	%rdi, %rdx
	addq	(%rsi), %rdx
	movl	-8(%rcx), %ecx
	addl	16(%rdx), %ecx
	movl	%ecx, %ecx
                                        # kill: def $rcx killed $ecx
	movl	(%rcx), %ecx
	movq	-32(%rax), %rax
	movslq	%ecx, %rcx
	movl	$1, (%rax,%rcx,4)
	movq	try_framesize.6@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	try_framesize.6@GOTPCREL(%rip), %rdx
	movq	%rdi, %rcx
	addq	(%rdx), %rcx
	movl	-8(%rcx), %ecx
	addl	$7, %ecx
	movq	try_framesize.6@GOTPCREL(%rip), %rsi
	movq	%rdi, %rdx
	addq	(%rsi), %rdx
	subl	16(%rdx), %ecx
	movl	%ecx, %ecx
                                        # kill: def $rcx killed $ecx
	movl	(%rcx), %ecx
	movq	-40(%rax), %rax
	movslq	%ecx, %rcx
	movl	$1, (%rax,%rcx,4)
	movq	try_framesize.6@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movl	-8(%rax), %edx
	movq	try_framesize.6@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	try_framesize.6@GOTPCREL(%rip), %rsi
	movq	%rdi, %rcx
	addq	(%rsi), %rcx
	movl	16(%rcx), %ecx
	movq	-24(%rax), %rax
	movslq	%ecx, %rcx
	movl	%edx, (%rax,%rcx,4)
	movq	try_framesize.6@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movl	16(%rax), %edx
	addl	$1, %edx
	movq	try_framesize.6@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rsi
	callq	try@PLT
	movq	16(%rsp), %rdx                  # 8-byte Reload
	movq	try_framesize.6@GOTPCREL(%rip), %rcx
	movq	%rdx, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	try_framesize.6@GOTPCREL(%rip), %rsi
	movq	%rdx, %rcx
	addq	(%rsi), %rcx
	movl	-8(%rcx), %ecx
	movq	-16(%rax), %rax
	movslq	%ecx, %rcx
	movl	$0, (%rax,%rcx,4)
	movq	try_framesize.6@GOTPCREL(%rip), %rcx
	movq	%rdx, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	try_framesize.6@GOTPCREL(%rip), %rsi
	movq	%rdx, %rcx
	addq	(%rsi), %rcx
	movq	try_framesize.6@GOTPCREL(%rip), %rdi
	movq	%rdx, %rsi
	addq	(%rdi), %rsi
	movl	-8(%rcx), %ecx
	addl	16(%rsi), %ecx
	movl	%ecx, %ecx
                                        # kill: def $rcx killed $ecx
	movl	(%rcx), %ecx
	movq	-32(%rax), %rax
	movslq	%ecx, %rcx
	movl	$0, (%rax,%rcx,4)
	movq	try_framesize.6@GOTPCREL(%rip), %rcx
	movq	%rdx, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	try_framesize.6@GOTPCREL(%rip), %rsi
	movq	%rdx, %rcx
	addq	(%rsi), %rcx
	movl	-8(%rcx), %ecx
	addl	$7, %ecx
	movq	try_framesize.6@GOTPCREL(%rip), %rsi
	addq	(%rsi), %rdx
	subl	16(%rdx), %ecx
	movl	%ecx, %ecx
                                        # kill: def $rcx killed $ecx
	movl	(%rcx), %ecx
	movq	-40(%rax), %rax
	movslq	%ecx, %rcx
	movl	$0, (%rax,%rcx,4)
.LBB2_6:                                # %if_next
                                        #   in Loop: Header=BB2_4 Depth=1
	jmp	.LBB2_3
.LBB2_7:                                # %for_exit
	jmp	.LBB2_8
.LBB2_8:                                # %if_next4
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end2:
	.size	try, .Lfunc_end2-try
	.cfi_endproc
                                        # -- End function
	.type	tigermain_framesize.1,@object   # @tigermain_framesize.1
	.section	.rodata,"a",@progbits
	.globl	tigermain_framesize.1
	.p2align	3
tigermain_framesize.1:
	.quad	64                              # 0x40
	.size	tigermain_framesize.1, 8

	.type	printboard_framesize.2,@object  # @printboard_framesize.2
	.globl	printboard_framesize.2
	.p2align	3
printboard_framesize.2:
	.quad	40                              # 0x28
	.size	printboard_framesize.2, 8

	.type	.L__unnamed_1,@object           # @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	" O"
	.size	.L__unnamed_1, 3

	.type	str,@object                     # @str
	.section	.data.rel.ro,"aw",@progbits
	.globl	str
	.p2align	3
str:
	.long	2                               # 0x2
	.zero	4
	.quad	.L__unnamed_1
	.size	str, 16

	.type	.L__unnamed_2,@object           # @1
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_2:
	.asciz	" ."
	.size	.L__unnamed_2, 3

	.type	str.3,@object                   # @str.3
	.section	.data.rel.ro,"aw",@progbits
	.globl	str.3
	.p2align	3
str.3:
	.long	2                               # 0x2
	.zero	4
	.quad	.L__unnamed_2
	.size	str.3, 16

	.type	.L__unnamed_3,@object           # @2
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_3:
	.asciz	"\n"
	.size	.L__unnamed_3, 2

	.type	str.4,@object                   # @str.4
	.section	.data.rel.ro,"aw",@progbits
	.globl	str.4
	.p2align	3
str.4:
	.long	1                               # 0x1
	.zero	4
	.quad	.L__unnamed_3
	.size	str.4, 16

	.type	.L__unnamed_4,@object           # @3
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_4:
	.asciz	"\n"
	.size	.L__unnamed_4, 2

	.type	str.5,@object                   # @str.5
	.section	.data.rel.ro,"aw",@progbits
	.globl	str.5
	.p2align	3
str.5:
	.long	1                               # 0x1
	.zero	4
	.quad	.L__unnamed_4
	.size	str.5, 16

	.type	try_framesize.6,@object         # @try_framesize.6
	.section	.rodata,"a",@progbits
	.globl	try_framesize.6
	.p2align	3
try_framesize.6:
	.quad	32                              # 0x20
	.size	try_framesize.6, 8

	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym print
	.addrsig_sym init_array
	.addrsig_sym printboard
	.addrsig_sym try
	.addrsig_sym tigermain_framesize.1
	.addrsig_sym printboard_framesize.2
	.addrsig_sym str
	.addrsig_sym str.3
	.addrsig_sym str.4
	.addrsig_sym str.5
	.addrsig_sym try_framesize.6
