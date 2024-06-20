	.text
	.file	"mila"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$192, %rsp
	.cfi_def_cfa_offset 208
	.cfi_offset %rbx, -16
	movl	$0, 24(%rsp)
	movl	$-20, 8(%rsp)
	cmpl	$20, 8(%rsp)
	jg	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:                                # =>This Inner Loop Header: Depth=1
	movl	8(%rsp), %eax
	leal	20(%rax), %ecx
	movslq	%ecx, %rcx
	movl	$0, 28(%rsp,%rcx,4)
	incl	%eax
	movl	%eax, 8(%rsp)
	cmpl	$20, 8(%rsp)
	jle	.LBB0_2
.LBB0_3:
	leaq	16(%rsp), %rdi
	callq	readln@PLT
	movl	$0, 8(%rsp)
	leaq	20(%rsp), %rbx
	.p2align	4, 0x90
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	movl	16(%rsp), %eax
	decl	%eax
	cmpl	%eax, 8(%rsp)
	jg	.LBB0_6
# %bb.5:                                #   in Loop: Header=BB0_4 Depth=1
	movq	%rbx, %rdi
	callq	readln@PLT
	movl	20(%rsp), %eax
	addl	$20, %eax
	cltq
	incl	28(%rsp,%rax,4)
	incl	8(%rsp)
	jmp	.LBB0_4
.LBB0_6:
	movl	$0, 12(%rsp)
	movl	$20, 8(%rsp)
	cmpl	$-20, 8(%rsp)
	jl	.LBB0_9
	.p2align	4, 0x90
.LBB0_8:                                # =>This Inner Loop Header: Depth=1
	movl	8(%rsp), %eax
	leal	20(%rax), %ecx
	movslq	%ecx, %rcx
	movl	28(%rsp,%rcx,4), %ecx
	imull	%eax, %ecx
	addl	%ecx, 12(%rsp)
	decl	%eax
	movl	%eax, 8(%rsp)
	cmpl	$-20, 8(%rsp)
	jge	.LBB0_8
.LBB0_9:
	movl	12(%rsp), %eax
	cltd
	idivl	16(%rsp)
	movl	%eax, %edi
	callq	writeln@PLT
	movl	24(%rsp), %eax
	addq	$192, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
