	.text

	.globl	main
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$72, %r8
	.comm	h,1,1
	movb	%r8b, h(%rip)
	movq	$69, %r8
	.comm	e,1,1
	movb	%r8b, e(%rip)
	movq	$76, %r8
	.comm	l,1,1
	movb	%r8b, l(%rip)
	movq	$79, %r8
	.comm	o,1,1
	movb	%r8b, o(%rip)
	movq	$87, %r8
	.comm	w,1,1
	movb	%r8b, w(%rip)
	movq	$82, %r8
	.comm	r,1,1
	movb	%r8b, r(%rip)
	movq	$68, %r8
	.comm	d,1,1
	movb	%r8b, d(%rip)
	movq	$32, %r8
	.comm	space,1,1
	movb	%r8b, space(%rip)
	movq	$33, %r8
	.comm	excl,1,1
	movb	%r8b, excl(%rip)
	movq	$10, %r8
	.comm	new_line,1,1
	movb	%r8b, new_line(%rip)
	movzbq	h(%rip), %r8
	movl	%r8d, %ecx
	call	print_byte
	movq	%rax, %r9
	movzbq	e(%rip), %r8
	movl	%r8d, %ecx
	call	print_byte
	movq	%rax, %r9
	movzbq	l(%rip), %r8
	movl	%r8d, %ecx
	call	print_byte
	movq	%rax, %r9
	movzbq	l(%rip), %r8
	movl	%r8d, %ecx
	call	print_byte
	movq	%rax, %r9
	movzbq	o(%rip), %r8
	movl	%r8d, %ecx
	call	print_byte
	movq	%rax, %r9
	movzbq	space(%rip), %r8
	movl	%r8d, %ecx
	call	print_byte
	movq	%rax, %r9
	movzbq	w(%rip), %r8
	movl	%r8d, %ecx
	call	print_byte
	movq	%rax, %r9
	movzbq	o(%rip), %r8
	movl	%r8d, %ecx
	call	print_byte
	movq	%rax, %r9
	movzbq	r(%rip), %r8
	movl	%r8d, %ecx
	call	print_byte
	movq	%rax, %r9
	movzbq	l(%rip), %r8
	movl	%r8d, %ecx
	call	print_byte
	movq	%rax, %r9
	movzbq	d(%rip), %r8
	movl	%r8d, %ecx
	call	print_byte
	movq	%rax, %r9
	movzbq	excl(%rip), %r8
	movl	%r8d, %ecx
	call	print_byte
	movq	%rax, %r9
	movzbq	new_line(%rip), %r8
	movl	%r8d, %ecx
	call	print_byte
	movq	%rax, %r9
	movq	$0, %r8
	movq	%r8, %rax
	popq	%rbp
	ret
