	.file	"xwindow.c"
	.text
	.globl	gpDisplay
	.bss
	.align 8
	.type	gpDisplay, @object
	.size	gpDisplay, 8
gpDisplay:
	.zero	8
	.globl	visualInfo
	.align 32
	.type	visualInfo, @object
	.size	visualInfo, 64
visualInfo:
	.zero	64
	.globl	window
	.align 8
	.type	window, @object
	.size	window, 8
window:
	.zero	8
	.globl	colormap
	.align 8
	.type	colormap, @object
	.size	colormap, 8
colormap:
	.zero	8
	.section	.rodata
	.align 8
.LC0:
	.string	"ERROR: XOpenDisplay() Failed to connect with XServer"
	.align 8
.LC1:
	.string	"ERROR: XMatchVisualInfo() Failed"
.LC2:
	.string	"ERROR: XCreateWindow() Failed"
.LC3:
	.string	"WM_DELETE_WINDOW"
	.align 8
.LC4:
	.string	"Mayur S Tadekar - XWindow - First Window"
	.text
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r12
	pushq	%rbx
	subq	$352, %rsp
	.cfi_offset 12, -24
	.cfi_offset 3, -32
	movq	%fs:40, %rax
	movq	%rax, -24(%rbp)
	xorl	%eax, %eax
	movl	$0, -356(%rbp)
	movl	$0, %edi
	call	XOpenDisplay@PLT
	movq	%rax, gpDisplay(%rip)
	movq	gpDisplay(%rip), %rax
	testq	%rax, %rax
	jne	.L2
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	call	uninitialize
	movl	$1, %edi
	call	exit@PLT
.L2:
	movq	gpDisplay(%rip), %rax
	movq	%rax, %rdi
	call	XDefaultScreen@PLT
	movl	%eax, -352(%rbp)
	movq	gpDisplay(%rip), %rax
	movl	-352(%rbp), %edx
	movl	%edx, %esi
	movq	%rax, %rdi
	call	XDefaultDepth@PLT
	movl	%eax, -348(%rbp)
	movl	$64, %edx
	movl	$0, %esi
	leaq	visualInfo(%rip), %rax
	movq	%rax, %rdi
	call	memset@PLT
	movq	gpDisplay(%rip), %rax
	movl	-348(%rbp), %edx
	movl	-352(%rbp), %esi
	leaq	visualInfo(%rip), %r8
	movl	$4, %ecx
	movq	%rax, %rdi
	call	XMatchVisualInfo@PLT
	movl	%eax, -356(%rbp)
	cmpl	$0, -356(%rbp)
	jne	.L3
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	call	uninitialize
	movl	$1, %edi
	call	exit@PLT
.L3:
	leaq	-336(%rbp), %rax
	movl	$112, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	$0, -312(%rbp)
	movq	$0, -336(%rbp)
	movl	16+visualInfo(%rip), %edx
	movq	gpDisplay(%rip), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	XBlackPixel@PLT
	movq	%rax, -328(%rbp)
	movq	visualInfo(%rip), %rbx
	movl	16+visualInfo(%rip), %edx
	movq	gpDisplay(%rip), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	XRootWindow@PLT
	movq	%rax, %rsi
	movq	gpDisplay(%rip), %rax
	movl	$0, %ecx
	movq	%rbx, %rdx
	movq	%rax, %rdi
	call	XCreateColormap@PLT
	movq	%rax, -240(%rbp)
	movq	visualInfo(%rip), %r12
	movl	20+visualInfo(%rip), %ebx
	movl	16+visualInfo(%rip), %edx
	movq	gpDisplay(%rip), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	XRootWindow@PLT
	movq	%rax, %rsi
	movq	gpDisplay(%rip), %rax
	leaq	-336(%rbp), %rdx
	pushq	%rdx
	pushq	$10250
	pushq	%r12
	pushq	$1
	pushq	%rbx
	pushq	$0
	movl	$600, %r9d
	movl	$800, %r8d
	movl	$0, %ecx
	movl	$0, %edx
	movq	%rax, %rdi
	call	XCreateWindow@PLT
	addq	$48, %rsp
	movq	%rax, window(%rip)
	movq	window(%rip), %rax
	testq	%rax, %rax
	jne	.L4
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	call	uninitialize
	movl	$1, %edi
	call	exit@PLT
.L4:
	movq	gpDisplay(%rip), %rax
	movl	$1, %edx
	leaq	.LC3(%rip), %rcx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	XInternAtom@PLT
	movq	%rax, -344(%rbp)
	movq	window(%rip), %rsi
	movq	gpDisplay(%rip), %rax
	leaq	-344(%rbp), %rdx
	movl	$1, %ecx
	movq	%rax, %rdi
	call	XSetWMProtocols@PLT
	movq	window(%rip), %rcx
	movq	gpDisplay(%rip), %rax
	leaq	.LC4(%rip), %rdx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	XStoreName@PLT
	movq	window(%rip), %rdx
	movq	gpDisplay(%rip), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	XMapWindow@PLT
.L6:
	movq	gpDisplay(%rip), %rax
	leaq	-224(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	XNextEvent@PLT
	movl	-224(%rbp), %eax
	cmpl	$33, %eax
	jne	.L9
	call	uninitialize
	movl	$0, %edi
	call	exit@PLT
.L9:
	nop
	jmp	.L6
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.globl	uninitialize
	.type	uninitialize, @function
uninitialize:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	window(%rip), %rax
	testq	%rax, %rax
	je	.L11
	movq	window(%rip), %rdx
	movq	gpDisplay(%rip), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	XDestroyWindow@PLT
.L11:
	movq	colormap(%rip), %rax
	testq	%rax, %rax
	je	.L12
	movq	colormap(%rip), %rdx
	movq	gpDisplay(%rip), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	XFreeColormap@PLT
.L12:
	movq	gpDisplay(%rip), %rax
	testq	%rax, %rax
	je	.L14
	movq	gpDisplay(%rip), %rax
	movq	%rax, %rdi
	call	XCloseDisplay@PLT
	movq	$0, gpDisplay(%rip)
.L14:
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	uninitialize, .-uninitialize
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
