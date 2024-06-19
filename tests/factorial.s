	.text
	.file	"mila"
	.globl	facti                           # -- Begin function facti
	.p2align	4, 0x90
	.type	facti,@function
facti:                                  # @facti
	.cfi_startproc
# %bb.0:                                # %entry
	movl	%edi, -4(%rsp)
	movl	$1, -8(%rsp)
	cmpl	$2, -4(%rsp)
	jb	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:                                # =>This Inner Loop Header: Depth=1
	movl	-4(%rsp), %eax
	movl	-8(%rsp), %ecx
	imull	%eax, %ecx
	movl	%ecx, -8(%rsp)
	decl	%eax
	movl	%eax, -4(%rsp)
	cmpl	$2, -4(%rsp)
	jae	.LBB0_2
.LBB0_3:
	movl	-8(%rsp), %eax
	retq
.Lfunc_end0:
	.size	facti, .Lfunc_end0-facti
	.cfi_endproc
                                        # -- End function
	.globl	factr                           # -- Begin function factr
	.p2align	4, 0x90
	.type	factr,@function
factr:                                  # @factr
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	.cfi_offset %rbx, -16
	movl	%edi, 12(%rsp)
	movl	$0, 8(%rsp)
	cmpl	$1, %edi
	jne	.LBB1_2
# %bb.1:
	movl	$1, 8(%rsp)
	jmp	.LBB1_3
.LBB1_2:
	movl	12(%rsp), %ebx
	leal	-1(%rbx), %edi
	callq	factr@PLT
	imull	%ebx, %eax
	movl	%eax, 8(%rsp)
.LBB1_3:
	movl	8(%rsp), %eax
	addq	$16, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	factr, .Lfunc_end1-factr
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
	movl	$5, %edi
	callq	facti@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$5, %edi
	callq	factr@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	4(%rsp), %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end2:
	.size	main, .Lfunc_end2-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
