.section    .note.GNU-stack,"",@progbits
	.text
.LC0:
	.string	"%d\n"
printint:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	ret

	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$0, %r8
	.comm	i,8,8
	movq	%r8, i(%rip)
	movq	$0, %r8
	.comm	sum,8,8
	movq	%r8, sum(%rip)
L0:
	movq	i(%rip), %r8
	movq	$100, %r9
	cmpq	%r8, %r9
	jle	L1
	movq	sum(%rip), %r8
	movq	i(%rip), %r9
	addq	%r8, %r9
	movq	%r9, sum(%rip)
	movq	i(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, i(%rip)
	jmp	L0
L1:
	movq	sum(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$0, %r8
	movq	%r8, i(%rip)
L2:
	movq	i(%rip), %r8
	movq	$1000000, %r9
	cmpq	%r8, %r9
	jle	L3
	movq	i(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	i(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, i(%rip)
	jmp	L2
L3:
	movl	$0, %eax
	popq	%rbp
	ret
