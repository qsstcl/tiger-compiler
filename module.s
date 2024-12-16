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
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rax
	subq	(%rax), %rdi
	movq	%rdi, (%rsp)                    # 8-byte Spill
	callq	__wrap_getchar@PLT
	movq	(%rsp), %rsi                    # 8-byte Reload
	movq	%rax, %rcx
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	movq	%rsi, %rax
	addq	(%rdx), %rax
	movq	%rcx, -8(%rax)
	movq	%rsi, %rdi
	callq	readlist@PLT
	movq	%rax, %rcx
	movq	(%rsp), %rax                    # 8-byte Reload
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rax
	movq	%rcx, -16(%rax)
	callq	__wrap_getchar@PLT
	movq	(%rsp), %rsi                    # 8-byte Reload
	movq	%rax, %rcx
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	movq	%rsi, %rax
	addq	(%rdx), %rax
	movq	%rcx, -8(%rax)
	movq	%rsi, %rdi
	callq	readlist@PLT
	movq	(%rsp), %rsi                    # 8-byte Reload
	movq	%rax, %rcx
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	movq	%rsi, %rax
	addq	(%rdx), %rax
	movq	%rcx, -24(%rax)
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	movq	%rsi, %rax
	addq	(%rcx), %rax
	movq	-16(%rax), %rdx
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	movq	%rsi, %rax
	addq	(%rcx), %rax
	movq	-24(%rax), %rcx
	movq	%rsi, %rdi
	callq	merge@PLT
	movq	(%rsp), %rsi                    # 8-byte Reload
	movq	%rax, %rdx
	movq	%rsi, %rdi
	callq	printlist@PLT
	xorl	%eax, %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	tigermain, .Lfunc_end0-tigermain
	.cfi_endproc
                                        # -- End function
	.globl	readint                         # -- Begin function readint
	.p2align	4, 0x90
	.type	readint,@function
readint:                                # @readint
	.cfi_startproc
# %bb.0:                                # %readint
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movq	%rsi, %rcx
	movq	%rdi, %rsi
	movq	readint_framesize.2@GOTPCREL(%rip), %rax
	subq	(%rax), %rsi
	movq	%rsi, 16(%rsp)                  # 8-byte Spill
	movq	readint_framesize.2@GOTPCREL(%rip), %rdi
	movq	%rsi, %rax
	addq	(%rdi), %rax
	movq	%rcx, 8(%rax)
	movq	%rdx, 16(%rax)
	movq	readint_framesize.2@GOTPCREL(%rip), %rcx
	movq	%rsi, %rax
	addq	(%rcx), %rax
	movl	$0, -8(%rax)
	movq	%rsi, %rdi
	callq	skipto@PLT
	movq	16(%rsp), %rsi                  # 8-byte Reload
	movq	readint_framesize.2@GOTPCREL(%rip), %rcx
	movq	%rsi, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	-8(%rax), %rdx
	movq	%rsi, %rdi
	callq	isdigit@PLT
	movl	%eax, %ecx
	movq	16(%rsp), %rax                  # 8-byte Reload
	movq	readint_framesize.2@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rax
	movq	16(%rax), %rax
	movl	%ecx, (%rax)
.LBB1_1:                                # %while_test
                                        # =>This Inner Loop Header: Depth=1
	movq	16(%rsp), %rsi                  # 8-byte Reload
	movq	readint_framesize.2@GOTPCREL(%rip), %rcx
	movq	%rsi, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	-8(%rax), %rdx
	movq	%rsi, %rdi
	callq	isdigit@PLT
	cmpl	$0, %eax
	je	.LBB1_3
# %bb.2:                                # %while_body
                                        #   in Loop: Header=BB1_1 Depth=1
	movq	16(%rsp), %rax                  # 8-byte Reload
	movq	readint_framesize.2@GOTPCREL(%rip), %rdx
	movq	%rax, %rcx
	addq	(%rdx), %rcx
	imull	$10, -8(%rcx), %ecx
	movl	%ecx, 8(%rsp)                   # 4-byte Spill
	movq	readint_framesize.2@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	-8(%rax), %rdi
	callq	ord@PLT
	movl	%eax, %ecx
	movl	8(%rsp), %eax                   # 4-byte Reload
	addl	%ecx, %eax
	movl	%eax, 12(%rsp)                  # 4-byte Spill
	movq	str.8@GOTPCREL(%rip), %rdi
	callq	ord@PLT
	movl	12(%rsp), %ecx                  # 4-byte Reload
	movl	%eax, %edx
	movq	16(%rsp), %rax                  # 8-byte Reload
	subl	%edx, %ecx
	movq	readint_framesize.2@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rax
	movl	%ecx, -8(%rax)
	callq	__wrap_getchar@PLT
	movq	%rax, %rcx
	movq	16(%rsp), %rax                  # 8-byte Reload
	movq	readint_framesize.2@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rax
	movq	%rcx, -8(%rax)
	jmp	.LBB1_1
.LBB1_3:                                # %while_next
	movq	16(%rsp), %rax                  # 8-byte Reload
	movq	readint_framesize.2@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movl	-8(%rax), %eax
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	readint, .Lfunc_end1-readint
	.cfi_endproc
                                        # -- End function
	.globl	isdigit                         # -- Begin function isdigit
	.p2align	4, 0x90
	.type	isdigit,@function
isdigit:                                # @isdigit
	.cfi_startproc
# %bb.0:                                # %isdigit
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movq	%rdi, %rax
	movq	isdigit_framesize.3@GOTPCREL(%rip), %rcx
	subq	(%rcx), %rax
	movq	%rax, 8(%rsp)                   # 8-byte Spill
	movq	isdigit_framesize.3@GOTPCREL(%rip), %rdi
	movq	%rax, %rcx
	addq	(%rdi), %rcx
	movq	%rsi, 8(%rcx)
	movq	%rdx, 16(%rcx)
	movq	isdigit_framesize.3@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	readint_framesize.2@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	-8(%rax), %rdi
	callq	ord@PLT
	movl	%eax, 4(%rsp)                   # 4-byte Spill
	movq	str@GOTPCREL(%rip), %rdi
	callq	ord@PLT
	movl	4(%rsp), %ecx                   # 4-byte Reload
	movl	%eax, %edx
	movq	8(%rsp), %rax                   # 8-byte Reload
	cmpl	%edx, %ecx
	setge	%cl
	movb	%cl, 23(%rsp)                   # 1-byte Spill
	movq	isdigit_framesize.3@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	readint_framesize.2@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	-8(%rax), %rdi
	callq	ord@PLT
	movl	%eax, 16(%rsp)                  # 4-byte Spill
	movq	str.4@GOTPCREL(%rip), %rdi
	callq	ord@PLT
	movl	16(%rsp), %ecx                  # 4-byte Reload
	movl	%eax, %edx
	movb	23(%rsp), %al                   # 1-byte Reload
	cmpl	%edx, %ecx
	setle	%cl
	andb	%cl, %al
	andb	$1, %al
	movzbl	%al, %eax
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end2:
	.size	isdigit, .Lfunc_end2-isdigit
	.cfi_endproc
                                        # -- End function
	.globl	skipto                          # -- Begin function skipto
	.p2align	4, 0x90
	.type	skipto,@function
skipto:                                 # @skipto
	.cfi_startproc
# %bb.0:                                # %skipto
	subq	$40, %rsp
	.cfi_def_cfa_offset 48
	movq	%rdi, %rax
	movq	skipto_framesize.5@GOTPCREL(%rip), %rcx
	subq	(%rcx), %rax
	movq	%rax, 32(%rsp)                  # 8-byte Spill
	movq	skipto_framesize.5@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	%rsi, 8(%rax)
.LBB3_1:                                # %while_test
                                        # =>This Inner Loop Header: Depth=1
	movq	32(%rsp), %rax                  # 8-byte Reload
	movq	skipto_framesize.5@GOTPCREL(%rip), %rcx
	movq	%rcx, (%rsp)                    # 8-byte Spill
	movq	(%rcx), %rcx
	movq	8(%rax,%rcx), %rax
	movq	readint_framesize.2@GOTPCREL(%rip), %rcx
	movq	%rcx, 8(%rsp)                   # 8-byte Spill
	movq	(%rcx), %rcx
	movq	8(%rax,%rcx), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rcx
	movq	%rcx, 16(%rsp)                  # 8-byte Spill
	movq	(%rcx), %rcx
	movq	-8(%rax,%rcx), %rdi
	movq	str.6@GOTPCREL(%rip), %rsi
	callq	string_equal@PLT
	movq	(%rsp), %rsi                    # 8-byte Reload
	movq	8(%rsp), %rdx                   # 8-byte Reload
	movq	16(%rsp), %rcx                  # 8-byte Reload
	movb	%al, %dil
	movq	32(%rsp), %rax                  # 8-byte Reload
	movb	%dil, 31(%rsp)                  # 1-byte Spill
	movq	(%rsi), %rsi
	movq	8(%rax,%rsi), %rax
	movq	(%rdx), %rdx
	movq	8(%rax,%rdx), %rax
	movq	(%rcx), %rcx
	movq	-8(%rax,%rcx), %rdi
	movq	str.7@GOTPCREL(%rip), %rsi
	callq	string_equal@PLT
	movb	%al, %cl
	movb	31(%rsp), %al                   # 1-byte Reload
	orb	%cl, %al
	testb	$1, %al
	je	.LBB3_3
	jmp	.LBB3_2
.LBB3_2:                                # %while_body
                                        #   in Loop: Header=BB3_1 Depth=1
	callq	__wrap_getchar@PLT
	movq	%rax, %rcx
	movq	32(%rsp), %rax                  # 8-byte Reload
	movq	skipto_framesize.5@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rax
	movq	8(%rax), %rax
	movq	readint_framesize.2@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rax
	movq	8(%rax), %rax
	movq	tigermain_framesize.1@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rax
	movq	%rcx, -8(%rax)
	jmp	.LBB3_1
.LBB3_3:                                # %while_next
	addq	$40, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end3:
	.size	skipto, .Lfunc_end3-skipto
	.cfi_endproc
                                        # -- End function
	.globl	readlist                        # -- Begin function readlist
	.p2align	4, 0x90
	.type	readlist,@function
readlist:                               # @readlist
	.cfi_startproc
# %bb.0:                                # %readlist
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movq	%rdi, %rax
	movq	readlist_framesize.9@GOTPCREL(%rip), %rcx
	subq	(%rcx), %rax
	movq	%rax, 16(%rsp)                  # 8-byte Spill
	movq	readlist_framesize.9@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	%rsi, 8(%rax)
	movl	$8, %edi
	callq	alloc_record@PLT
	movq	16(%rsp), %rdi                  # 8-byte Reload
	movq	%rax, %rcx
	movl	$0, (%rcx)
	movq	readlist_framesize.9@GOTPCREL(%rip), %rdx
	movq	%rdi, %rax
	addq	(%rdx), %rax
	movq	%rcx, -8(%rax)
	movq	readlist_framesize.9@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	-8(%rax), %rdx
	movq	readlist_framesize.9@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rsi
	callq	readint@PLT
	movl	%eax, %edx
	movq	16(%rsp), %rax                  # 8-byte Reload
	movq	readlist_framesize.9@GOTPCREL(%rip), %rsi
	movq	%rax, %rcx
	addq	(%rsi), %rcx
	movl	%edx, -16(%rcx)
	movq	readlist_framesize.9@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	-8(%rax), %rax
	cmpl	$0, (%rax)
	je	.LBB4_2
# %bb.1:                                # %if_then3
	movl	$16, %edi
	callq	alloc_record@PLT
	movq	16(%rsp), %rdi                  # 8-byte Reload
	movq	%rax, (%rsp)                    # 8-byte Spill
	movq	readlist_framesize.9@GOTPCREL(%rip), %rdx
	movq	%rdi, %rcx
	addq	(%rdx), %rcx
	movl	-16(%rcx), %ecx
	movl	%ecx, (%rax)
	movq	readlist_framesize.9@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rsi
	callq	readlist@PLT
	movq	%rax, %rcx
	movq	(%rsp), %rax                    # 8-byte Reload
	movq	%rcx, 8(%rax)
	movq	%rax, 8(%rsp)                   # 8-byte Spill
	jmp	.LBB4_3
.LBB4_2:                                # %if_else
	xorl	%eax, %eax
                                        # kill: def $rax killed $eax
	movq	%rax, 8(%rsp)                   # 8-byte Spill
	jmp	.LBB4_3
.LBB4_3:                                # %if_next
	movq	8(%rsp), %rax                   # 8-byte Reload
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end4:
	.size	readlist, .Lfunc_end4-readlist
	.cfi_endproc
                                        # -- End function
	.globl	merge                           # -- Begin function merge
	.p2align	4, 0x90
	.type	merge,@function
merge:                                  # @merge
	.cfi_startproc
# %bb.0:                                # %merge
	subq	$72, %rsp
	.cfi_def_cfa_offset 80
	movq	%rcx, 40(%rsp)                  # 8-byte Spill
	movq	%rdx, %rax
	movq	40(%rsp), %rdx                  # 8-byte Reload
	movq	%rax, 48(%rsp)                  # 8-byte Spill
	movq	%rsi, %rax
	movq	48(%rsp), %rsi                  # 8-byte Reload
	movq	%rax, 56(%rsp)                  # 8-byte Spill
	movq	%rdi, %rax
	movq	56(%rsp), %rdi                  # 8-byte Reload
	movq	merge_framesize.10@GOTPCREL(%rip), %rcx
	subq	(%rcx), %rax
	movq	%rax, 64(%rsp)                  # 8-byte Spill
	movq	merge_framesize.10@GOTPCREL(%rip), %r8
	movq	%rax, %rcx
	addq	(%r8), %rcx
	movq	%rdi, 8(%rcx)
	movq	%rsi, 16(%rcx)
	movq	%rdx, 24(%rcx)
	movq	merge_framesize.10@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	cmpq	$0, 16(%rax)
	jne	.LBB5_2
# %bb.1:                                # %if_then2
	movq	64(%rsp), %rax                  # 8-byte Reload
	movq	merge_framesize.10@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	24(%rax), %rax
	movq	%rax, 32(%rsp)                  # 8-byte Spill
	jmp	.LBB5_9
.LBB5_2:                                # %if_else
	movq	64(%rsp), %rax                  # 8-byte Reload
	movq	merge_framesize.10@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	cmpq	$0, 24(%rax)
	jne	.LBB5_4
# %bb.3:                                # %if_then3
	movq	64(%rsp), %rax                  # 8-byte Reload
	movq	merge_framesize.10@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movq	16(%rax), %rax
	movq	%rax, 24(%rsp)                  # 8-byte Spill
	jmp	.LBB5_8
.LBB5_4:                                # %if_else4
	movq	64(%rsp), %rcx                  # 8-byte Reload
	movq	merge_framesize.10@GOTPCREL(%rip), %rdx
	movq	%rcx, %rax
	addq	(%rdx), %rax
	movq	16(%rax), %rax
	movq	merge_framesize.10@GOTPCREL(%rip), %rdx
	addq	(%rdx), %rcx
	movq	24(%rcx), %rcx
	movl	(%rax), %eax
	cmpl	(%rcx), %eax
	jge	.LBB5_6
# %bb.5:                                # %if_then6
	movl	$16, %edi
	callq	alloc_record@PLT
	movq	64(%rsp), %rdi                  # 8-byte Reload
	movq	%rax, 8(%rsp)                   # 8-byte Spill
	movq	merge_framesize.10@GOTPCREL(%rip), %rdx
	movq	%rdi, %rcx
	addq	(%rdx), %rcx
	movq	16(%rcx), %rcx
	movl	(%rcx), %ecx
	movl	%ecx, (%rax)
	movq	merge_framesize.10@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	16(%rax), %rax
	movq	8(%rax), %rdx
	movq	merge_framesize.10@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	24(%rax), %rcx
	movq	merge_framesize.10@GOTPCREL(%rip), %rsi
	movq	%rdi, %rax
	addq	(%rsi), %rax
	movq	8(%rax), %rsi
	callq	merge@PLT
	movq	%rax, %rcx
	movq	8(%rsp), %rax                   # 8-byte Reload
	movq	%rcx, 8(%rax)
	movq	%rax, 16(%rsp)                  # 8-byte Spill
	jmp	.LBB5_7
.LBB5_6:                                # %if_else10
	movl	$16, %edi
	callq	alloc_record@PLT
	movq	64(%rsp), %rdi                  # 8-byte Reload
	movq	%rax, (%rsp)                    # 8-byte Spill
	movq	merge_framesize.10@GOTPCREL(%rip), %rdx
	movq	%rdi, %rcx
	addq	(%rdx), %rcx
	movq	24(%rcx), %rcx
	movl	(%rcx), %ecx
	movl	%ecx, (%rax)
	movq	merge_framesize.10@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	16(%rax), %rdx
	movq	merge_framesize.10@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rcx
	movq	merge_framesize.10@GOTPCREL(%rip), %rsi
	movq	%rdi, %rax
	addq	(%rsi), %rax
	movq	8(%rax), %rsi
	callq	merge@PLT
	movq	%rax, %rcx
	movq	(%rsp), %rax                    # 8-byte Reload
	movq	%rcx, 8(%rax)
	movq	%rax, 16(%rsp)                  # 8-byte Spill
.LBB5_7:                                # %if_next
	movq	16(%rsp), %rax                  # 8-byte Reload
	movq	%rax, 24(%rsp)                  # 8-byte Spill
.LBB5_8:                                # %if_next15
	movq	24(%rsp), %rax                  # 8-byte Reload
	movq	%rax, 32(%rsp)                  # 8-byte Spill
.LBB5_9:                                # %if_next17
	movq	32(%rsp), %rax                  # 8-byte Reload
	addq	$72, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end5:
	.size	merge, .Lfunc_end5-merge
	.cfi_endproc
                                        # -- End function
	.globl	printint                        # -- Begin function printint
	.p2align	4, 0x90
	.type	printint,@function
printint:                               # @printint
	.cfi_startproc
# %bb.0:                                # %printint
	pushq	%rax
	.cfi_def_cfa_offset 16
	movq	%rdi, %rax
	movq	printint_framesize.11@GOTPCREL(%rip), %rcx
	subq	(%rcx), %rax
	movq	%rax, (%rsp)                    # 8-byte Spill
	movq	printint_framesize.11@GOTPCREL(%rip), %rdi
	movq	%rax, %rcx
	addq	(%rdi), %rcx
	movq	%rsi, 8(%rcx)
	movl	%edx, 16(%rcx)
	movq	printint_framesize.11@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	cmpl	$0, 16(%rax)
	jge	.LBB6_2
# %bb.1:                                # %if_then2
	movq	str.14@GOTPCREL(%rip), %rdi
	callq	print@PLT
	movq	(%rsp), %rsi                    # 8-byte Reload
	movq	printint_framesize.11@GOTPCREL(%rip), %rcx
	movq	%rsi, %rax
	addq	(%rcx), %rax
	xorl	%edx, %edx
	subl	16(%rax), %edx
	movq	%rsi, %rdi
	callq	f@PLT
	jmp	.LBB6_6
.LBB6_2:                                # %if_else
	movq	(%rsp), %rax                    # 8-byte Reload
	movq	printint_framesize.11@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	cmpl	$0, 16(%rax)
	jle	.LBB6_4
# %bb.3:                                # %if_then3
	movq	(%rsp), %rsi                    # 8-byte Reload
	movq	printint_framesize.11@GOTPCREL(%rip), %rcx
	movq	%rsi, %rax
	addq	(%rcx), %rax
	movl	16(%rax), %edx
	movq	%rsi, %rdi
	callq	f@PLT
	jmp	.LBB6_5
.LBB6_4:                                # %if_else4
	movq	str.15@GOTPCREL(%rip), %rdi
	callq	print@PLT
.LBB6_5:                                # %if_next
	jmp	.LBB6_6
.LBB6_6:                                # %if_next5
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end6:
	.size	printint, .Lfunc_end6-printint
	.cfi_endproc
                                        # -- End function
	.globl	f                               # -- Begin function f
	.p2align	4, 0x90
	.type	f,@function
f:                                      # @f
	.cfi_startproc
# %bb.0:                                # %f
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movq	%rdi, %rax
	movq	f_framesize.12@GOTPCREL(%rip), %rcx
	subq	(%rcx), %rax
	movq	%rax, 16(%rsp)                  # 8-byte Spill
	movq	f_framesize.12@GOTPCREL(%rip), %rdi
	movq	%rax, %rcx
	addq	(%rdi), %rcx
	movq	%rsi, 8(%rcx)
	movl	%edx, 16(%rcx)
	movq	f_framesize.12@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	cmpl	$0, 16(%rax)
	jle	.LBB7_2
# %bb.1:                                # %if_then2
	movq	16(%rsp), %rdi                  # 8-byte Reload
	movq	f_framesize.12@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movl	16(%rax), %eax
	movl	$10, %ecx
	cltd
	idivl	%ecx
	movl	%eax, %edx
	movq	f_framesize.12@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rsi
	callq	f@PLT
	movq	16(%rsp), %rax                  # 8-byte Reload
	movq	f_framesize.12@GOTPCREL(%rip), %rdx
	movq	%rax, %rcx
	addq	(%rdx), %rcx
	movq	%rcx, (%rsp)                    # 8-byte Spill
	movq	f_framesize.12@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	movl	16(%rax), %eax
	movl	$10, %ecx
	cltd
	idivl	%ecx
	movl	%eax, %ecx
	movq	(%rsp), %rax                    # 8-byte Reload
	imull	$10, %ecx, %ecx
	movl	16(%rax), %eax
	subl	%ecx, %eax
	movl	%eax, 12(%rsp)                  # 4-byte Spill
	movq	str.13@GOTPCREL(%rip), %rdi
	callq	ord@PLT
	movl	12(%rsp), %edi                  # 4-byte Reload
	addl	%eax, %edi
	callq	chr@PLT
	movq	%rax, %rdi
	callq	print@PLT
.LBB7_2:                                # %if_next
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end7:
	.size	f, .Lfunc_end7-f
	.cfi_endproc
                                        # -- End function
	.globl	printlist                       # -- Begin function printlist
	.p2align	4, 0x90
	.type	printlist,@function
printlist:                              # @printlist
	.cfi_startproc
# %bb.0:                                # %printlist
	pushq	%rax
	.cfi_def_cfa_offset 16
	movq	%rdi, %rax
	movq	printlist_framesize.16@GOTPCREL(%rip), %rcx
	subq	(%rcx), %rax
	movq	%rax, (%rsp)                    # 8-byte Spill
	movq	printlist_framesize.16@GOTPCREL(%rip), %rdi
	movq	%rax, %rcx
	addq	(%rdi), %rcx
	movq	%rsi, 8(%rcx)
	movq	%rdx, 16(%rcx)
	movq	printlist_framesize.16@GOTPCREL(%rip), %rcx
	addq	(%rcx), %rax
	cmpq	$0, 16(%rax)
	jne	.LBB8_2
# %bb.1:                                # %if_then2
	movq	str.17@GOTPCREL(%rip), %rdi
	callq	print@PLT
	jmp	.LBB8_3
.LBB8_2:                                # %if_else
	movq	(%rsp), %rdi                    # 8-byte Reload
	movq	printlist_framesize.16@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	16(%rax), %rax
	movl	(%rax), %edx
	movq	printlist_framesize.16@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rsi
	callq	printint@PLT
	movq	str.18@GOTPCREL(%rip), %rdi
	callq	print@PLT
	movq	(%rsp), %rdi                    # 8-byte Reload
	movq	printlist_framesize.16@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	16(%rax), %rax
	movq	8(%rax), %rdx
	movq	printlist_framesize.16@GOTPCREL(%rip), %rcx
	movq	%rdi, %rax
	addq	(%rcx), %rax
	movq	8(%rax), %rsi
	callq	printlist@PLT
.LBB8_3:                                # %if_next
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end8:
	.size	printlist, .Lfunc_end8-printlist
	.cfi_endproc
                                        # -- End function
	.type	tigermain_framesize.1,@object   # @tigermain_framesize.1
	.section	.rodata,"a",@progbits
	.globl	tigermain_framesize.1
	.p2align	3
tigermain_framesize.1:
	.quad	56                              # 0x38
	.size	tigermain_framesize.1, 8

	.type	readint_framesize.2,@object     # @readint_framesize.2
	.globl	readint_framesize.2
	.p2align	3
readint_framesize.2:
	.quad	32                              # 0x20
	.size	readint_framesize.2, 8

	.type	isdigit_framesize.3,@object     # @isdigit_framesize.3
	.globl	isdigit_framesize.3
	.p2align	3
isdigit_framesize.3:
	.quad	24                              # 0x18
	.size	isdigit_framesize.3, 8

	.type	.L__unnamed_1,@object           # @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"0"
	.size	.L__unnamed_1, 2

	.type	str,@object                     # @str
	.section	.data.rel.ro,"aw",@progbits
	.globl	str
	.p2align	3
str:
	.long	1                               # 0x1
	.zero	4
	.quad	.L__unnamed_1
	.size	str, 16

	.type	.L__unnamed_2,@object           # @1
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_2:
	.asciz	"9"
	.size	.L__unnamed_2, 2

	.type	str.4,@object                   # @str.4
	.section	.data.rel.ro,"aw",@progbits
	.globl	str.4
	.p2align	3
str.4:
	.long	1                               # 0x1
	.zero	4
	.quad	.L__unnamed_2
	.size	str.4, 16

	.type	skipto_framesize.5,@object      # @skipto_framesize.5
	.section	.rodata,"a",@progbits
	.globl	skipto_framesize.5
	.p2align	3
skipto_framesize.5:
	.quad	16                              # 0x10
	.size	skipto_framesize.5, 8

	.type	.L__unnamed_3,@object           # @2
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_3:
	.asciz	" "
	.size	.L__unnamed_3, 2

	.type	str.6,@object                   # @str.6
	.section	.data.rel.ro,"aw",@progbits
	.globl	str.6
	.p2align	3
str.6:
	.long	1                               # 0x1
	.zero	4
	.quad	.L__unnamed_3
	.size	str.6, 16

	.type	.L__unnamed_4,@object           # @3
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_4:
	.asciz	"\n"
	.size	.L__unnamed_4, 2

	.type	str.7,@object                   # @str.7
	.section	.data.rel.ro,"aw",@progbits
	.globl	str.7
	.p2align	3
str.7:
	.long	1                               # 0x1
	.zero	4
	.quad	.L__unnamed_4
	.size	str.7, 16

	.type	.L__unnamed_5,@object           # @4
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_5:
	.asciz	"0"
	.size	.L__unnamed_5, 2

	.type	str.8,@object                   # @str.8
	.section	.data.rel.ro,"aw",@progbits
	.globl	str.8
	.p2align	3
str.8:
	.long	1                               # 0x1
	.zero	4
	.quad	.L__unnamed_5
	.size	str.8, 16

	.type	readlist_framesize.9,@object    # @readlist_framesize.9
	.section	.rodata,"a",@progbits
	.globl	readlist_framesize.9
	.p2align	3
readlist_framesize.9:
	.quad	40                              # 0x28
	.size	readlist_framesize.9, 8

	.type	merge_framesize.10,@object      # @merge_framesize.10
	.globl	merge_framesize.10
	.p2align	3
merge_framesize.10:
	.quad	32                              # 0x20
	.size	merge_framesize.10, 8

	.type	printint_framesize.11,@object   # @printint_framesize.11
	.globl	printint_framesize.11
	.p2align	3
printint_framesize.11:
	.quad	24                              # 0x18
	.size	printint_framesize.11, 8

	.type	f_framesize.12,@object          # @f_framesize.12
	.globl	f_framesize.12
	.p2align	3
f_framesize.12:
	.quad	24                              # 0x18
	.size	f_framesize.12, 8

	.type	.L__unnamed_6,@object           # @5
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_6:
	.asciz	"0"
	.size	.L__unnamed_6, 2

	.type	str.13,@object                  # @str.13
	.section	.data.rel.ro,"aw",@progbits
	.globl	str.13
	.p2align	3
str.13:
	.long	1                               # 0x1
	.zero	4
	.quad	.L__unnamed_6
	.size	str.13, 16

	.type	.L__unnamed_7,@object           # @6
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_7:
	.asciz	"-"
	.size	.L__unnamed_7, 2

	.type	str.14,@object                  # @str.14
	.section	.data.rel.ro,"aw",@progbits
	.globl	str.14
	.p2align	3
str.14:
	.long	1                               # 0x1
	.zero	4
	.quad	.L__unnamed_7
	.size	str.14, 16

	.type	.L__unnamed_8,@object           # @7
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_8:
	.asciz	"0"
	.size	.L__unnamed_8, 2

	.type	str.15,@object                  # @str.15
	.section	.data.rel.ro,"aw",@progbits
	.globl	str.15
	.p2align	3
str.15:
	.long	1                               # 0x1
	.zero	4
	.quad	.L__unnamed_8
	.size	str.15, 16

	.type	printlist_framesize.16,@object  # @printlist_framesize.16
	.section	.rodata,"a",@progbits
	.globl	printlist_framesize.16
	.p2align	3
printlist_framesize.16:
	.quad	24                              # 0x18
	.size	printlist_framesize.16, 8

	.type	.L__unnamed_9,@object           # @8
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_9:
	.asciz	"\n"
	.size	.L__unnamed_9, 2

	.type	str.17,@object                  # @str.17
	.section	.data.rel.ro,"aw",@progbits
	.globl	str.17
	.p2align	3
str.17:
	.long	1                               # 0x1
	.zero	4
	.quad	.L__unnamed_9
	.size	str.17, 16

	.type	.L__unnamed_10,@object          # @9
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_10:
	.asciz	" "
	.size	.L__unnamed_10, 2

	.type	str.18,@object                  # @str.18
	.section	.data.rel.ro,"aw",@progbits
	.globl	str.18
	.p2align	3
str.18:
	.long	1                               # 0x1
	.zero	4
	.quad	.L__unnamed_10
	.size	str.18, 16

	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym chr
	.addrsig_sym __wrap_getchar
	.addrsig_sym print
	.addrsig_sym ord
	.addrsig_sym alloc_record
	.addrsig_sym string_equal
	.addrsig_sym readint
	.addrsig_sym isdigit
	.addrsig_sym skipto
	.addrsig_sym readlist
	.addrsig_sym merge
	.addrsig_sym printint
	.addrsig_sym f
	.addrsig_sym printlist
	.addrsig_sym tigermain_framesize.1
	.addrsig_sym readint_framesize.2
	.addrsig_sym isdigit_framesize.3
	.addrsig_sym str
	.addrsig_sym str.4
	.addrsig_sym skipto_framesize.5
	.addrsig_sym str.6
	.addrsig_sym str.7
	.addrsig_sym str.8
	.addrsig_sym readlist_framesize.9
	.addrsig_sym merge_framesize.10
	.addrsig_sym printint_framesize.11
	.addrsig_sym f_framesize.12
	.addrsig_sym str.13
	.addrsig_sym str.14
	.addrsig_sym str.15
	.addrsig_sym printlist_framesize.16
	.addrsig_sym str.17
	.addrsig_sym str.18
