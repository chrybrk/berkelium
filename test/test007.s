.section    .note.GNU-stack,"",@progbits
	.text

	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$723, %r8
	.comm	number,4,4
	movl	%r8d, number(%rip)
	movslq	number(%rip), %r8
	.comm	original_number,4,4
	movl	%r8d, original_number(%rip)
	movq	$0, %r8
	.comm	reversed,4,4
	movl	%r8d, reversed(%rip)
L0:
	movslq	number(%rip), %r8
	movq	$0, %r9
	cmpq	%r8, %r9
	je	L1
	movslq	number(%rip), %r8
	movq	$10, %r9
	movq	%r8,%rax
	cltd
	idivq	%r9
	movsxd	%edx,%r8
	.comm	last_digit,4,4
	movl	%r8d, last_digit(%rip)
	movslq	reversed(%rip), %r8
	movq	$10, %r9
	imulq	%r8, %r9
	.comm	shit_size,4,4
	movl	%r9d, shit_size(%rip)
	movslq	shit_size(%rip), %r8
	movslq	last_digit(%rip), %r9
	addq	%r8, %r9
	movl	%r9d, reversed(%rip)
	movslq	number(%rip), %r8
	movq	$10, %r9
	movq	%r8,%rax
	cqo
	idivq	%r9
	movq	%rax,%r8
	movl	%r8d, number(%rip)
	jmp	L0
L1:
	movslq	original_number(%rip), %r8
	movq	%r8, %rdi
	call	print_i32
	movq	%rax, %r9
	movq	$10, %r8
	movq	%r8, %rdi
	call	print_byte
	movq	%rax, %r9
	movslq	reversed(%rip), %r8
	movq	%r8, %rdi
	call	print_i32
	movq	%rax, %r9
	movq	$10, %r8
	movq	%r8, %rdi
	call	print_byte
	movq	%rax, %r9
L2:
	movslq	reversed(%rip), %r8
	movq	$0, %r9
	cmpq	%r8, %r9
	je	L3
	movslq	reversed(%rip), %r8
	movq	$10, %r9
	movq	%r8,%rax
	cltd
	idivq	%r9
	movsxd	%edx,%r8
	movq	%r8, %rdi
	call	print_i32
	movq	%rax, %r9
	movq	$32, %r8
	movq	%r8, %rdi
	call	print_byte
	movq	%rax, %r9
	movslq	reversed(%rip), %r8
	movq	$10, %r9
	movq	%r8,%rax
	cqo
	idivq	%r9
	movq	%rax,%r8
	movl	%r8d, reversed(%rip)
	jmp	L2
L3:
	movq	$0, %r8
	movq	%r8, %rax
	popq	%rbp
	ret
