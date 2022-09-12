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
	movq	$10, %r8
	.comm	i,4,4
	movl	%r8d, i(%rip)
L0:
	movslq	i(%rip), %r8
	movq	$0, %r9
	cmpq	%r8, %r9
	jge	L1
	movq	$0, %r8
	.comm	j,4,4
	movl	%r8d, j(%rip)
L2:
	movslq	j(%rip), %r8
	movslq	i(%rip), %r9
	cmpq	%r8, %r9
	jle	L3
	movslq	j(%rip), %r8
	movq	%r8, %rdi
	call	print_i32
	movq	%rax, %r9
	movslq	j(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movl	%r9d, j(%rip)
	jmp	L2
L3:
	movq	$0, %r8
	movq	%r8, %rdi
	call	ln
	movq	%rax, %r9
	movslq	i(%rip), %r8
	movq	$1, %r9
	subq	%r9, %r8
	movl	%r8d, i(%rip)
	jmp	L0
L1:
	movq	$0, %r8
	movq	%r8, %rax
	popq	%rbp
	ret
