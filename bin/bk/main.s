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
	.comm	x,8,8
	movq	$7, %r8
	movq	$9, %r9
	cmpq	%r9, %r8
	setl	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$7, %r8
	movq	$9, %r9
	cmpq	%r9, %r8
	setle	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$7, %r8
	movq	$9, %r9
	cmpq	%r9, %r8
	setne	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$7, %r8
	movq	$7, %r9
	cmpq	%r9, %r8
	sete	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$7, %r8
	movq	$7, %r9
	cmpq	%r9, %r8
	setge	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$7, %r8
	movq	$7, %r9
	cmpq	%r9, %r8
	setle	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$9, %r8
	movq	$7, %r9
	cmpq	%r9, %r8
	setg	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$9, %r8
	movq	$7, %r9
	cmpq	%r9, %r8
	setge	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$9, %r8
	movq	$7, %r9
	cmpq	%r9, %r8
	setne	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movl	$0, %eax
	popq	%rbp
	ret
