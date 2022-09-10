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

	.globl	test
	.type	test, @function
test:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$400, %r8
	movl	%r8d, %eax
	jmp	L0
L0:
	popq	%rbp
	ret
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$10, %r8
	movq	%r8, %rdi
	call	printint
	movq	$1, %r8
	movq	%r8, %rdi
	call	test
	movq	%rax, %r9
	movq	$400, %r8
	addq	%r9, %r8
	movq	%r8, %rdi
	call	printint
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq	%rbp
	ret
