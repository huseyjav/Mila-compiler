	.text
	.file	"mila"
	.globl	fact                            # -- Begin function fact
	.p2align	4, 0x90
	.type	fact,@function
fact:                                   # @fact
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	.cfi_offset %rbx, -16
	movl	%edi, 12(%rsp)
	movl	$0, 8(%rsp)
	testl	%edi, %edi
	jne	.LBB0_2
# %bb.1:
	movl	$1, 8(%rsp)
	jmp	.LBB0_3
.LBB0_2:
	movl	12(%rsp), %ebx
	leal	-1(%rbx), %edi
	callq	fact@PLT
	imull	%ebx, %eax
	movl	%eax, 8(%rsp)
.LBB0_3:
	movl	8(%rsp), %eax
	addq	$16, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	fact, .Lfunc_end0-fact
	.cfi_endproc
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$0, 4(%rsp)
	movq	%rsp, %rdi
	callq	readln@PLT
	movl	(%rsp), %edi
	callq	fact@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	4(%rsp), %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
