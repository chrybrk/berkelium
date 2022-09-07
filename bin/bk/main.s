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
	movq	$69000, %r8
	.comm	term,8,8
	movq	%r8, term(%rip)
	movq	$0, %r8
	.comm	first,8,8
	movq	%r8, first(%rip)
	movq	$1, %r8
	.comm	second,8,8
	movq	%r8, second(%rip)
	movq	$0, %r8
	.comm	i,8,8
	movq	%r8, i(%rip)
L0:
	movq	i(%rip), %r8
	movq	term(%rip), %r9
	cmpq	%r8, %r9
	jl	L1
	movq	first(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	first(%rip), %r8
	movq	second(%rip), %r9
	addq	%r8, %r9
	.comm	next,8,8
	movq	%r9, next(%rip)
	movq	second(%rip), %r8
	movq	%r8, first(%rip)
	movq	next(%rip), %r8
	movq	%r8, second(%rip)
	movq	i(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, i(%rip)
	jmp	L0
L1:
	movl	$0, %eax
	popq	%rbp
	ret
