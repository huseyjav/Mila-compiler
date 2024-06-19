	.text
	.file	"mila"
	.globl	isodd                           # -- Begin function isodd
	.p2align	4, 0x90
	.type	isodd,@function
isodd:                                  # @isodd
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	%edi, 4(%rsp)
	movl	$0, (%rsp)
	testl	%edi, %edi
	je	.LBB0_2
# %bb.1:
	movl	4(%rsp), %edi
	decl	%edi
	callq	iseven@PLT
	movl	%eax, (%rsp)
	jmp	.LBB0_3
.LBB0_2:
	movl	$0, (%rsp)
.LBB0_3:
	movl	(%rsp), %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	isodd, .Lfunc_end0-isodd
	.cfi_endproc
                                        # -- End function
	.globl	iseven                          # -- Begin function iseven
	.p2align	4, 0x90
	.type	iseven,@function
iseven:                                 # @iseven
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	%edi, 4(%rsp)
	movl	$0, (%rsp)
	testl	%edi, %edi
	je	.LBB1_2
# %bb.1:
	movl	4(%rsp), %edi
	decl	%edi
	callq	isodd@PLT
	movl	%eax, (%rsp)
	jmp	.LBB1_3
.LBB1_2:
	movl	$1, (%rsp)
.LBB1_3:
	movl	(%rsp), %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	iseven, .Lfunc_end1-iseven
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
	movl	$11, %edi
	callq	iseven@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$11, %edi
	callq	isodd@PLT
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
