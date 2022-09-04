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
	.comm	node,8,8
	movq	%r8, node(%rip)
L0:
	movq	node(%rip), %r8
	movq	$10, %r9
	cmpq	%r8, %r9
	jle	L1
	movq	$0, %r8
	.comm	next,8,8
	movq	%r8, next(%rip)
L2:
	movq	next(%rip), %r8
	movq	node(%rip), %r9
	cmpq	%r8, %r9
	jle	L3
	movq	next(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	next(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, next(%rip)
	jmp	L2
L3:
	movq	node(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, node(%rip)
	jmp	L0
L1:
	movl	$0, %eax
	popq	%rbp
	ret
