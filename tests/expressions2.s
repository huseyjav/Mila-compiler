	.text
	.file	"mila"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	subq	$40, %rsp
	.cfi_def_cfa_offset 48
	movl	$0, 28(%rsp)
	leaq	24(%rsp), %rdi
	callq	readln@PLT
	leaq	12(%rsp), %rdi
	callq	readln@PLT
	movl	12(%rsp), %eax
	movl	24(%rsp), %edi
	leal	(%rdi,%rax), %ecx
	movl	%ecx, 20(%rsp)
	subl	%edi, %eax
	movl	%eax, 16(%rsp)
                                        # kill: def $edi killed $edi killed $rdi
	callq	writeln@PLT
	movl	12(%rsp), %edi
	callq	writeln@PLT
	movl	20(%rsp), %edi
	callq	writeln@PLT
	movl	16(%rsp), %edi
	callq	writeln@PLT
	movl	24(%rsp), %eax
	addl	20(%rsp), %eax
	movl	12(%rsp), %edi
	subl	16(%rsp), %edi
	imull	%eax, %edi
	movl	%edi, 36(%rsp)
	callq	writeln@PLT
	movl	20(%rsp), %eax
	cltd
	idivl	16(%rsp)
	movl	%edx, %edi
	movl	%edx, 32(%rsp)
	callq	writeln@PLT
	movl	28(%rsp), %eax
	addq	$40, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
