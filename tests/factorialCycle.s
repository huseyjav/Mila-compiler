	.text
	.file	"mila"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movl	$0, 20(%rsp)
	movl	$1, 16(%rsp)
	leaq	12(%rsp), %rdi
	callq	readln@PLT
	cmpl	$2, 12(%rsp)
	jl	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:                                # =>This Inner Loop Header: Depth=1
	movl	12(%rsp), %eax
	movl	16(%rsp), %ecx
	imull	%eax, %ecx
	movl	%ecx, 16(%rsp)
	decl	%eax
	movl	%eax, 12(%rsp)
	cmpl	$2, 12(%rsp)
	jge	.LBB0_2
.LBB0_3:
	movl	16(%rsp), %edi
	callq	writeln@PLT
	movl	20(%rsp), %eax
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
