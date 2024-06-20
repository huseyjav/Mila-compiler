	.text
	.file	"mila"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	subq	$104, %rsp
	.cfi_def_cfa_offset 112
	movl	$0, 12(%rsp)
	movl	$0, 4(%rsp)
	cmpl	$20, 4(%rsp)
	jg	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:                                # =>This Inner Loop Header: Depth=1
	movslq	4(%rsp), %rax
	movl	$20, %ecx
	subl	%eax, %ecx
	movl	%ecx, 20(%rsp,%rax,8)
	incl	%eax
	movl	%eax, 4(%rsp)
	cmpl	$20, 4(%rsp)
	jle	.LBB0_2
.LBB0_3:
	movl	$0, 4(%rsp)
	cmpl	$20, 4(%rsp)
	jg	.LBB0_6
	.p2align	4, 0x90
.LBB0_5:                                # =>This Inner Loop Header: Depth=1
	movslq	4(%rsp), %rax
	movl	20(%rsp,%rax,8), %edi
	callq	writeln@PLT
	incl	4(%rsp)
	cmpl	$20, 4(%rsp)
	jle	.LBB0_5
.LBB0_6:
	movl	$1, 4(%rsp)
	jmp	.LBB0_7
	.p2align	4, 0x90
.LBB0_13:                               #   in Loop: Header=BB0_7 Depth=1
	incl	4(%rsp)
.LBB0_7:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_9 Depth 2
	cmpl	$20, 4(%rsp)
	jg	.LBB0_14
# %bb.8:                                #   in Loop: Header=BB0_7 Depth=1
	movl	$20, 8(%rsp)
	jmp	.LBB0_9
	.p2align	4, 0x90
.LBB0_12:                               #   in Loop: Header=BB0_9 Depth=2
	decl	8(%rsp)
.LBB0_9:                                #   Parent Loop BB0_7 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movl	8(%rsp), %eax
	cmpl	4(%rsp), %eax
	jl	.LBB0_13
# %bb.10:                               #   in Loop: Header=BB0_9 Depth=2
	movslq	8(%rsp), %rax
	movl	20(%rsp,%rax,8), %ecx
	decl	%eax
	cltq
	cmpl	20(%rsp,%rax,8), %ecx
	jge	.LBB0_12
# %bb.11:                               #   in Loop: Header=BB0_9 Depth=2
	movl	8(%rsp), %eax
	decl	%eax
	cltq
	movl	20(%rsp,%rax,8), %eax
	movl	%eax, 16(%rsp)
	movslq	8(%rsp), %rcx
	leal	-1(%rcx), %edx
	movslq	%edx, %rdx
	movl	20(%rsp,%rcx,8), %esi
	movl	%esi, 20(%rsp,%rdx,8)
	movl	%eax, 20(%rsp,%rcx,8)
	jmp	.LBB0_12
.LBB0_14:
	movl	$0, 4(%rsp)
	cmpl	$20, 4(%rsp)
	jg	.LBB0_17
	.p2align	4, 0x90
.LBB0_16:                               # =>This Inner Loop Header: Depth=1
	movslq	4(%rsp), %rax
	movl	20(%rsp,%rax,8), %edi
	callq	writeln@PLT
	incl	4(%rsp)
	cmpl	$20, 4(%rsp)
	jle	.LBB0_16
.LBB0_17:
	movl	12(%rsp), %eax
	addq	$104, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
