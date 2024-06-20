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
	movl	$0, 16(%rsp)
	movl	$11, 20(%rsp)
	movabsq	$549755813954, %rax             # imm = 0x8000000042
	movq	%rax, 24(%rsp)
	movabsq	$571230650417, %rax             # imm = 0x8500000031
	movq	%rax, 32(%rsp)
	movabsq	$64424509486, %rax              # imm = 0xF0000002E
	movq	%rax, 40(%rsp)
	movabsq	$236223201367, %rax             # imm = 0x3700000057
	movq	%rax, 48(%rsp)
	movl	$37, 56(%rsp)
	movabsq	$188978561102, %rax             # imm = 0x2C0000004E
	movq	%rax, 60(%rsp)
	movl	$33, 68(%rsp)
	movabsq	$365072220198, %rax             # imm = 0x5500000026
	movq	%rax, 72(%rsp)
	movl	$6, 80(%rsp)
	movabsq	$17179869334, %rax              # imm = 0x400000096
	movq	%rax, 84(%rsp)
	movl	$1, 92(%rsp)
	movabsq	$335007449143, %rax             # imm = 0x4E00000037
	movq	%rax, 96(%rsp)
	movl	$0, 8(%rsp)
	cmpl	$20, 8(%rsp)
	jg	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:                                # =>This Inner Loop Header: Depth=1
	movslq	8(%rsp), %rax
	movl	20(%rsp,%rax,4), %edi
	callq	writeln@PLT
	incl	8(%rsp)
	cmpl	$20, 8(%rsp)
	jle	.LBB0_2
.LBB0_3:
	movl	20(%rsp), %eax
	movl	%eax, 12(%rsp)
	movl	$1, 8(%rsp)
	jmp	.LBB0_4
	.p2align	4, 0x90
.LBB0_7:                                #   in Loop: Header=BB0_4 Depth=1
	incl	8(%rsp)
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	cmpl	$20, 8(%rsp)
	jg	.LBB0_8
# %bb.5:                                #   in Loop: Header=BB0_4 Depth=1
	movl	12(%rsp), %eax
	movslq	8(%rsp), %rcx
	cmpl	20(%rsp,%rcx,4), %eax
	jge	.LBB0_7
# %bb.6:                                #   in Loop: Header=BB0_4 Depth=1
	movslq	8(%rsp), %rax
	movl	20(%rsp,%rax,4), %eax
	movl	%eax, 12(%rsp)
	jmp	.LBB0_7
.LBB0_8:
	movl	12(%rsp), %edi
	callq	writeln@PLT
	movl	16(%rsp), %eax
	addq	$104, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
