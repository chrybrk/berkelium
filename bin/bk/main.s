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
	movq	$100, %r8
	.comm	t1,8,8
	movq	%r8, t1(%rip)
	movq	$120, %r8
	.comm	t2,8,8
	movq	%r8, t2(%rip)
	movq	t1(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	t2(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	t1(%rip), %r8
	movq	t2(%rip), %r9
	addq	%r8, %r9
	movq	%r9, %rdi
	call	printint
	movq	$10, %r8
	movq	$20, %r9
	imulq	%r8, %r9
	movq	%r9, %rdi
	call	printint
	movq	$10, %r8
	movq	$2, %r9
	movq	%r8,%rax
	cqo
	idivq	%r9
	movq	%rax,%r8
	movq	%r8, %rdi
	call	printint
	movl	$0, %eax
	popq	%rbp
	ret
