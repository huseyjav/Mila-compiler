	.text
	.file	"mila"
	.globl	isprime                         # -- Begin function isprime
	.p2align	4, 0x90
	.type	isprime,@function
isprime:                                # @isprime
	.cfi_startproc
# %bb.0:                                # %entry
	movl	%edi, -8(%rsp)
	movl	$0, -12(%rsp)
	cmpl	$1, %edi
	jg	.LBB0_1
.LBB0_7:
	movl	$0, -12(%rsp)
.LBB0_8:
	movl	-12(%rsp), %eax
	retq
.LBB0_1:
	cmpl	$3, -8(%rsp)
	jg	.LBB0_3
# %bb.2:
	movl	$1, -12(%rsp)
	movl	-12(%rsp), %eax
	retq
.LBB0_3:
	movl	-8(%rsp), %eax
	movl	%eax, %ecx
	shrl	$31, %ecx
	addl	%eax, %ecx
	andl	$-2, %ecx
	cmpl	%ecx, %eax
	setne	%cl
	imull	$-1431655765, %eax, %eax        # imm = 0xAAAAAAAB
	addl	$715827882, %eax                # imm = 0x2AAAAAAA
	cmpl	$1431655765, %eax               # imm = 0x55555555
	setae	%al
	testb	%al, %cl
	je	.LBB0_7
# %bb.4:
	movl	$1, -12(%rsp)
	movl	$5, -4(%rsp)
	.p2align	4, 0x90
.LBB0_5:                                # =>This Inner Loop Header: Depth=1
	movl	-4(%rsp), %eax
	cmpl	-8(%rsp), %eax
	jge	.LBB0_8
# %bb.6:                                #   in Loop: Header=BB0_5 Depth=1
	movl	-8(%rsp), %eax
	cltd
	idivl	-4(%rsp)
	testl	%edx, %edx
	je	.LBB0_7
# %bb.9:                                #   in Loop: Header=BB0_5 Depth=1
	addl	$2, -4(%rsp)
	jmp	.LBB0_5
.Lfunc_end0:
	.size	isprime, .Lfunc_end0-isprime
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
	xorl	%edi, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$1, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$2, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$3, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$4, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$5, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$6, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$7, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$8, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$9, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$10, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$11, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$12, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$13, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$14, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$15, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$16, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$17, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$119, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$187, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$101, %edi
	callq	isprime@PLT
	movl	%eax, %edi
	callq	writeln@PLT
	movl	$103, %edi
	callq	isprime@PLT
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
