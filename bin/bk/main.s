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
	movq	$23, %r8
	movq	$34, %r9
	imulq	%r8, %r9
	movq	%r9, %rdi
	call	printint
	movq	$34, %r8
	movq	%r8, %rdi
	call	printint
	movq	$23, %r8
	movq	$23, %r9
	imulq	%r8, %r9
	movq	$434, %r8
	addq	%r9, %r8
	movq	%r8, %rdi
	call	printint
	movl	$0, %eax
	popq	%rbp
	ret
