	.text
	.file	"mila"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$0, 4(%rsp)
	movl	$0, (%rsp)
	cmpl	$10, (%rsp)
	jg	.LBB0_4
	.p2align	4, 0x90
.LBB0_2:                                # =>This Inner Loop Header: Depth=1
	movl	(%rsp), %edi
	callq	writeln@PLT
	cmpl	$5, (%rsp)
	je	.LBB0_4
# %bb.3:                                #   in Loop: Header=BB0_2 Depth=1
	incl	(%rsp)
	cmpl	$10, (%rsp)
	jle	.LBB0_2
.LBB0_4:
	movl	4(%rsp), %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
