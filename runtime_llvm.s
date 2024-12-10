	.text
	.file	"runtime_llvm.c"
	.globl	sum_seven                       # -- Begin function sum_seven
	.p2align	4, 0x90
	.type	sum_seven,@function
sum_seven:                              # @sum_seven
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	16(%rbp), %eax
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	movl	%r8d, -20(%rbp)
	movl	%r9d, -24(%rbp)
	movl	-4(%rbp), %eax
	addl	-8(%rbp), %eax
	addl	-12(%rbp), %eax
	addl	-16(%rbp), %eax
	addl	-20(%rbp), %eax
	addl	-24(%rbp), %eax
	addl	16(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	sum_seven, .Lfunc_end0-sum_seven
	.cfi_endproc
                                        # -- End function
	.globl	init_array                      # -- Begin function init_array
	.p2align	4, 0x90
	.type	init_array,@function
init_array:                             # @init_array
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movslq	-4(%rbp), %rdi
	shlq	$3, %rdi
	callq	malloc@PLT
	movq	%rax, -32(%rbp)
	movl	$0, -20(%rbp)
.LBB1_1:                                # =>This Inner Loop Header: Depth=1
	movl	-20(%rbp), %eax
	cmpl	-4(%rbp), %eax
	jge	.LBB1_4
# %bb.2:                                #   in Loop: Header=BB1_1 Depth=1
	movq	-16(%rbp), %rdx
	movq	-32(%rbp), %rax
	movslq	-20(%rbp), %rcx
	movq	%rdx, (%rax,%rcx,8)
# %bb.3:                                #   in Loop: Header=BB1_1 Depth=1
	movl	-20(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -20(%rbp)
	jmp	.LBB1_1
.LBB1_4:
	movq	-32(%rbp), %rax
	addq	$32, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end1:
	.size	init_array, .Lfunc_end1-init_array
	.cfi_endproc
                                        # -- End function
	.globl	alloc_record                    # -- Begin function alloc_record
	.p2align	4, 0x90
	.type	alloc_record,@function
alloc_record:                           # @alloc_record
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movslq	-4(%rbp), %rdi
	callq	malloc@PLT
	movq	%rax, -24(%rbp)
	movq	%rax, -16(%rbp)
	movl	$0, -8(%rbp)
.LBB2_1:                                # =>This Inner Loop Header: Depth=1
	movl	-8(%rbp), %eax
	cmpl	-4(%rbp), %eax
	jge	.LBB2_4
# %bb.2:                                #   in Loop: Header=BB2_1 Depth=1
	movq	-16(%rbp), %rax
	movq	%rax, %rcx
	addq	$4, %rcx
	movq	%rcx, -16(%rbp)
	movl	$0, (%rax)
# %bb.3:                                #   in Loop: Header=BB2_1 Depth=1
	movslq	-8(%rbp), %rax
	addq	$4, %rax
                                        # kill: def $eax killed $eax killed $rax
	movl	%eax, -8(%rbp)
	jmp	.LBB2_1
.LBB2_4:
	movq	-24(%rbp), %rax
	addq	$32, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end2:
	.size	alloc_record, .Lfunc_end2-alloc_record
	.cfi_endproc
                                        # -- End function
	.globl	string_equal                    # -- Begin function string_equal
	.p2align	4, 0x90
	.type	string_equal,@function
string_equal:                           # @string_equal
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-16(%rbp), %rax
	cmpq	-24(%rbp), %rax
	jne	.LBB3_2
# %bb.1:
	movl	$1, -4(%rbp)
	jmp	.LBB3_11
.LBB3_2:
	movq	-16(%rbp), %rax
	movl	(%rax), %eax
	movq	-24(%rbp), %rcx
	cmpl	(%rcx), %eax
	je	.LBB3_4
# %bb.3:
	movl	$0, -4(%rbp)
	jmp	.LBB3_11
.LBB3_4:
	movl	$0, -28(%rbp)
.LBB3_5:                                # =>This Inner Loop Header: Depth=1
	movl	-28(%rbp), %eax
	movq	-16(%rbp), %rcx
	cmpl	(%rcx), %eax
	jge	.LBB3_10
# %bb.6:                                #   in Loop: Header=BB3_5 Depth=1
	movq	-16(%rbp), %rax
	movq	8(%rax), %rax
	movslq	-28(%rbp), %rcx
	movsbl	(%rax,%rcx), %eax
	movq	-24(%rbp), %rcx
	movq	8(%rcx), %rcx
	movslq	-28(%rbp), %rdx
	movsbl	(%rcx,%rdx), %ecx
	cmpl	%ecx, %eax
	je	.LBB3_8
# %bb.7:
	movl	$0, -4(%rbp)
	jmp	.LBB3_11
.LBB3_8:                                #   in Loop: Header=BB3_5 Depth=1
	jmp	.LBB3_9
.LBB3_9:                                #   in Loop: Header=BB3_5 Depth=1
	movl	-28(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -28(%rbp)
	jmp	.LBB3_5
.LBB3_10:
	movl	$1, -4(%rbp)
.LBB3_11:
	movl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end3:
	.size	string_equal, .Lfunc_end3-string_equal
	.cfi_endproc
                                        # -- End function
	.globl	print                           # -- Begin function print
	.p2align	4, 0x90
	.type	print,@function
print:                                  # @print
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, -24(%rbp)
	movl	$0, -12(%rbp)
.LBB4_1:                                # =>This Inner Loop Header: Depth=1
	movl	-12(%rbp), %eax
	movq	-8(%rbp), %rcx
	cmpl	(%rcx), %eax
	jge	.LBB4_4
# %bb.2:                                #   in Loop: Header=BB4_1 Depth=1
	movq	-24(%rbp), %rax
	movsbl	(%rax), %edi
	callq	putchar@PLT
# %bb.3:                                #   in Loop: Header=BB4_1 Depth=1
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	movq	-24(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB4_1
.LBB4_4:
	movq	stdout@GOTPCREL(%rip), %rax
	movq	(%rax), %rdi
	callq	fflush@PLT
	addq	$32, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end4:
	.size	print, .Lfunc_end4-print
	.cfi_endproc
                                        # -- End function
	.globl	flush                           # -- Begin function flush
	.p2align	4, 0x90
	.type	flush,@function
flush:                                  # @flush
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	stdout@GOTPCREL(%rip), %rax
	movq	(%rax), %rdi
	callq	fflush@PLT
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end5:
	.size	flush, .Lfunc_end5-flush
	.cfi_endproc
                                        # -- End function
	.globl	printi                          # -- Begin function printi
	.p2align	4, 0x90
	.type	printi,@function
printi:                                 # @printi
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %esi
	leaq	.L.str(%rip), %rdi
	movb	$0, %al
	callq	printf@PLT
	callq	flush
	addq	$16, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end6:
	.size	printi, .Lfunc_end6-printi
	.cfi_endproc
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
.LBB7_1:                                # =>This Inner Loop Header: Depth=1
	cmpl	$256, -8(%rbp)                  # imm = 0x100
	jge	.LBB7_4
# %bb.2:                                #   in Loop: Header=BB7_1 Depth=1
	movslq	-8(%rbp), %rcx
	leaq	consts(%rip), %rax
	shlq	$4, %rcx
	addq	%rcx, %rax
	movl	$1, (%rax)
	movl	$1, %edi
	callq	malloc@PLT
	movq	%rax, %rcx
	movslq	-8(%rbp), %rdx
	leaq	consts(%rip), %rax
	shlq	$4, %rdx
	addq	%rdx, %rax
	movq	%rcx, 8(%rax)
	movl	-8(%rbp), %eax
	movb	%al, %cl
	movslq	-8(%rbp), %rdx
	leaq	consts(%rip), %rax
	shlq	$4, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	movb	%cl, (%rax)
# %bb.3:                                #   in Loop: Header=BB7_1 Depth=1
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -8(%rbp)
	jmp	.LBB7_1
.LBB7_4:
	movl	$1048576, %edi                  # imm = 0x100000
	callq	malloc@PLT
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	addq	$1048576, %rax                  # imm = 0x100000
	andq	$-65536, %rax                   # imm = 0xFFFF0000
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rdi
	movq	-24(%rbp), %rsi
	callq	tigermain@PLT
	addq	$32, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end7:
	.size	main, .Lfunc_end7-main
	.cfi_endproc
                                        # -- End function
	.globl	ord                             # -- Begin function ord
	.p2align	4, 0x90
	.type	ord,@function
ord:                                    # @ord
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rax
	cmpl	$0, (%rax)
	jne	.LBB8_2
# %bb.1:
	movl	$-1, -4(%rbp)
	jmp	.LBB8_3
.LBB8_2:
	movq	-16(%rbp), %rax
	movq	8(%rax), %rax
	movsbl	(%rax), %eax
	movl	%eax, -4(%rbp)
.LBB8_3:
	movl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end8:
	.size	ord, .Lfunc_end8-ord
	.cfi_endproc
                                        # -- End function
	.globl	chr                             # -- Begin function chr
	.p2align	4, 0x90
	.type	chr,@function
chr:                                    # @chr
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	cmpl	$0, -4(%rbp)
	jl	.LBB9_2
# %bb.1:
	cmpl	$256, -4(%rbp)                  # imm = 0x100
	jl	.LBB9_3
.LBB9_2:
	movl	-4(%rbp), %esi
	leaq	.L.str.2(%rip), %rdi
	movb	$0, %al
	callq	printf@PLT
	movl	$1, %edi
	callq	exit@PLT
.LBB9_3:
	movslq	-4(%rbp), %rcx
	leaq	consts(%rip), %rax
	shlq	$4, %rcx
	addq	%rcx, %rax
	addq	$16, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end9:
	.size	chr, .Lfunc_end9-chr
	.cfi_endproc
                                        # -- End function
	.globl	size                            # -- Begin function size
	.p2align	4, 0x90
	.type	size,@function
size:                                   # @size
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end10:
	.size	size, .Lfunc_end10-size
	.cfi_endproc
                                        # -- End function
	.globl	substring                       # -- Begin function substring
	.p2align	4, 0x90
	.type	substring,@function
substring:                              # @substring
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	%rdi, -16(%rbp)
	movl	%esi, -20(%rbp)
	movl	%edx, -24(%rbp)
	cmpl	$0, -20(%rbp)
	jl	.LBB11_2
# %bb.1:
	movl	-20(%rbp), %eax
	addl	-24(%rbp), %eax
	movq	-16(%rbp), %rcx
	cmpl	(%rcx), %eax
	jle	.LBB11_3
.LBB11_2:
	movq	-16(%rbp), %rax
	movl	(%rax), %esi
	movl	-20(%rbp), %edx
	movl	-24(%rbp), %ecx
	leaq	.L.str.3(%rip), %rdi
	movb	$0, %al
	callq	printf@PLT
	movl	$1, %edi
	callq	exit@PLT
.LBB11_3:
	cmpl	$1, -24(%rbp)
	jne	.LBB11_5
# %bb.4:
	movq	-16(%rbp), %rax
	movq	8(%rax), %rax
	movslq	-20(%rbp), %rcx
	movsbl	(%rax,%rcx), %eax
	movslq	%eax, %rcx
	leaq	consts(%rip), %rax
	shlq	$4, %rcx
	addq	%rcx, %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB11_10
.LBB11_5:
	movl	$16, %edi
	callq	malloc@PLT
	movq	%rax, -32(%rbp)
	movslq	-24(%rbp), %rdi
	callq	malloc@PLT
	movq	%rax, %rcx
	movq	-32(%rbp), %rax
	movq	%rcx, 8(%rax)
	movl	-24(%rbp), %ecx
	movq	-32(%rbp), %rax
	movl	%ecx, (%rax)
	movl	$0, -36(%rbp)
.LBB11_6:                               # =>This Inner Loop Header: Depth=1
	movl	-36(%rbp), %eax
	cmpl	-24(%rbp), %eax
	jge	.LBB11_9
# %bb.7:                                #   in Loop: Header=BB11_6 Depth=1
	movq	-16(%rbp), %rax
	movq	8(%rax), %rax
	movl	-20(%rbp), %ecx
	addl	-36(%rbp), %ecx
	movslq	%ecx, %rcx
	movb	(%rax,%rcx), %dl
	movq	-32(%rbp), %rax
	movq	8(%rax), %rax
	movslq	-36(%rbp), %rcx
	movb	%dl, (%rax,%rcx)
# %bb.8:                                #   in Loop: Header=BB11_6 Depth=1
	movl	-36(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -36(%rbp)
	jmp	.LBB11_6
.LBB11_9:
	movq	-32(%rbp), %rax
	movq	%rax, -8(%rbp)
.LBB11_10:
	movq	-8(%rbp), %rax
	addq	$48, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end11:
	.size	substring, .Lfunc_end11-substring
	.cfi_endproc
                                        # -- End function
	.globl	concat                          # -- Begin function concat
	.p2align	4, 0x90
	.type	concat,@function
concat:                                 # @concat
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-16(%rbp), %rax
	cmpl	$0, (%rax)
	jne	.LBB12_2
# %bb.1:
	movq	-24(%rbp), %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB12_13
.LBB12_2:
	movq	-24(%rbp), %rax
	cmpl	$0, (%rax)
	jne	.LBB12_4
# %bb.3:
	movq	-16(%rbp), %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB12_13
.LBB12_4:
	movq	-16(%rbp), %rax
	movl	(%rax), %eax
	movq	-24(%rbp), %rcx
	addl	(%rcx), %eax
	movl	%eax, -32(%rbp)
	movl	$16, %edi
	callq	malloc@PLT
	movq	%rax, -40(%rbp)
	movslq	-32(%rbp), %rdi
	callq	malloc@PLT
	movq	%rax, %rcx
	movq	-40(%rbp), %rax
	movq	%rcx, 8(%rax)
	movl	-32(%rbp), %ecx
	movq	-40(%rbp), %rax
	movl	%ecx, (%rax)
	movl	$0, -28(%rbp)
.LBB12_5:                               # =>This Inner Loop Header: Depth=1
	movl	-28(%rbp), %eax
	movq	-16(%rbp), %rcx
	cmpl	(%rcx), %eax
	jge	.LBB12_8
# %bb.6:                                #   in Loop: Header=BB12_5 Depth=1
	movq	-16(%rbp), %rax
	movq	8(%rax), %rax
	movslq	-28(%rbp), %rcx
	movb	(%rax,%rcx), %dl
	movq	-40(%rbp), %rax
	movq	8(%rax), %rax
	movslq	-28(%rbp), %rcx
	movb	%dl, (%rax,%rcx)
# %bb.7:                                #   in Loop: Header=BB12_5 Depth=1
	movl	-28(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -28(%rbp)
	jmp	.LBB12_5
.LBB12_8:
	movl	$0, -28(%rbp)
.LBB12_9:                               # =>This Inner Loop Header: Depth=1
	movl	-28(%rbp), %eax
	movq	-24(%rbp), %rcx
	cmpl	(%rcx), %eax
	jge	.LBB12_12
# %bb.10:                               #   in Loop: Header=BB12_9 Depth=1
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	movslq	-28(%rbp), %rcx
	movb	(%rax,%rcx), %dl
	movq	-40(%rbp), %rax
	movq	8(%rax), %rax
	movl	-28(%rbp), %ecx
	movq	-16(%rbp), %rsi
	addl	(%rsi), %ecx
	movslq	%ecx, %rcx
	movb	%dl, (%rax,%rcx)
# %bb.11:                               #   in Loop: Header=BB12_9 Depth=1
	movl	-28(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -28(%rbp)
	jmp	.LBB12_9
.LBB12_12:
	movq	-40(%rbp), %rax
	movq	%rax, -8(%rbp)
.LBB12_13:
	movq	-8(%rbp), %rax
	addq	$48, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end12:
	.size	concat, .Lfunc_end12-concat
	.cfi_endproc
                                        # -- End function
	.globl	not                             # -- Begin function not
	.p2align	4, 0x90
	.type	not,@function
not:                                    # @not
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	cmpl	$0, -4(%rbp)
	setne	%al
	xorb	$-1, %al
	andb	$1, %al
	movzbl	%al, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end13:
	.size	not, .Lfunc_end13-not
	.cfi_endproc
                                        # -- End function
	.globl	__wrap_getchar                  # -- Begin function __wrap_getchar
	.p2align	4, 0x90
	.type	__wrap_getchar,@function
__wrap_getchar:                         # @__wrap_getchar
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	stdin@GOTPCREL(%rip), %rax
	movq	(%rax), %rdi
	callq	getc@PLT
	movl	%eax, -12(%rbp)
	cmpl	$-1, -12(%rbp)
	jne	.LBB14_2
# %bb.1:
	leaq	empty(%rip), %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB14_3
.LBB14_2:
	movslq	-12(%rbp), %rcx
	leaq	consts(%rip), %rax
	shlq	$4, %rcx
	addq	%rcx, %rax
	movq	%rax, -8(%rbp)
.LBB14_3:
	movq	-8(%rbp), %rax
	addq	$16, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end14:
	.size	__wrap_getchar, .Lfunc_end14-__wrap_getchar
	.cfi_endproc
                                        # -- End function
	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"%d"
	.size	.L.str, 3

	.type	.L.str.1,@object                # @.str.1
.L.str.1:
	.zero	1
	.size	.L.str.1, 1

	.type	empty,@object                   # @empty
	.data
	.globl	empty
	.p2align	3
empty:
	.long	0                               # 0x0
	.zero	4
	.quad	.L.str.1
	.size	empty, 16

	.type	consts,@object                  # @consts
	.bss
	.globl	consts
	.p2align	4
consts:
	.zero	4096
	.size	consts, 4096

	.type	.L.str.2,@object                # @.str.2
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.2:
	.asciz	"chr(%d) out of range\n"
	.size	.L.str.2, 22

	.type	.L.str.3,@object                # @.str.3
.L.str.3:
	.asciz	"substring([%d],%d,%d) out of range\n"
	.size	.L.str.3, 36

	.ident	"Ubuntu clang version 14.0.0-1ubuntu1.1"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym malloc
	.addrsig_sym putchar
	.addrsig_sym fflush
	.addrsig_sym flush
	.addrsig_sym printf
	.addrsig_sym tigermain
	.addrsig_sym exit
	.addrsig_sym getc
	.addrsig_sym stdout
	.addrsig_sym empty
	.addrsig_sym consts
	.addrsig_sym stdin
