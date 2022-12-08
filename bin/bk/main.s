.section    .note.GNU-stack,"",@progbits
	.text

	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$10, %r8
	.comm	a,4,4
	movl	%r8d, a(%rip)
	movq	$20, %r8
	.comm	b,4,4
	movl	%r8d, b(%rip)
	movslq	a(%rip), %r8
	movq	%r8, %rdi
	call	print_i32
	movq	%rax, %r9
	movq	$32, %r8
	movq	%r8, %rdi
	call	print_byte
	movq	%rax, %r9
	movslq	b(%rip), %r8
	movq	%r8, %rdi
	call	print_i32
	movq	%rax, %r9
	movq	$0, %r8
	movq	%r8, %rax
	popq	%rbp
	ret
