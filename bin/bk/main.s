.section    .note.GNU-stack,"",@progbits
	.text

	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$10, %r8
	.comm	a,8,8
	movq	%r8, a(%rip)
	leaq	a(%rip), %r8
	.comm	b,8,8
	movq	%r8, b(%rip)
	movq	b(%rip), %r8
	movq	(%r8), %r8
	.comm	c,8,8
	movq	%r8, c(%rip)
	movq	a(%rip), %r8
	movq	%r8, %rdi
	call	print_i32
	movq	%rax, %r9
	movq	$10, %r8
	movq	%r8, %rdi
	call	print_byte
	movq	%rax, %r9
	movq	c(%rip), %r8
	movq	%r8, %rdi
	call	print_i32
	movq	%rax, %r9
	movq	$0, %r8
	movq	%r8, %rax
	popq	%rbp
	ret
