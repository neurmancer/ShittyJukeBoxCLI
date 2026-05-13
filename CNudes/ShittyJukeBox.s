	.file	"ShittyJukeBox.c"
	.text
	.globl	myTherapySession
	.bss
	.align 32
	.type	myTherapySession, @object
	.size	myTherapySession, 40
myTherapySession:
	.zero	40
	.globl	adhdSongs
	.align 32
	.type	adhdSongs, @object
	.size	adhdSongs, 40
adhdSongs:
	.zero	40
	.globl	nwobhmSongs
	.align 32
	.type	nwobhmSongs, @object
	.size	nwobhmSongs, 40
nwobhmSongs:
	.zero	40
	.globl	whiteGirlPopSongs
	.align 32
	.type	whiteGirlPopSongs, @object
	.size	whiteGirlPopSongs, 40
whiteGirlPopSongs:
	.zero	40
	.globl	genreSongArrays
	.section	.data.rel.local,"aw"
	.align 32
	.type	genreSongArrays, @object
	.size	genreSongArrays, 40
genreSongArrays:
	.quad	0
	.quad	myTherapySession
	.quad	adhdSongs
	.quad	nwobhmSongs
	.quad	whiteGirlPopSongs
	.globl	rickroll
	.section	.rodata
.LC0:
	.string	""
	.section	.data.rel.local
	.align 8
	.type	rickroll, @object
	.size	rickroll, 8
rickroll:
	.quad	.LC0
	.globl	rickrollAudio
	.section	.rodata
	.align 8
.LC1:
	.string	"https://drive.google.com/uc?export=open&id=10aiePDkX2t2XruCaFROIum9nGkQGUzth"
	.section	.data.rel.local
	.align 8
	.type	rickrollAudio, @object
	.size	rickrollAudio, 8
rickrollAudio:
	.quad	.LC1
	.globl	roast
	.section	.rodata
	.align 8
.LC2:
	.string	"https://drive.google.com/uc?export=open&id=1LXCZzIJ_LR9Thw8-M2oVnKfYCHUn5obY"
	.section	.data.rel.local
	.align 8
	.type	roast, @object
	.size	roast, 8
roast:
	.quad	.LC2
	.globl	meaLuxAudio
	.section	.rodata
	.align 8
.LC3:
	.string	"https://drive.google.com/uc?export=open&id=1oG209grBSgu_q6MxCbw-L9wMNBw5HKOe"
	.section	.data.rel.local
	.align 8
	.type	meaLuxAudio, @object
	.size	meaLuxAudio, 8
meaLuxAudio:
	.quad	.LC3
	.globl	allGenres
	.bss
	.align 16
	.type	allGenres, @object
	.size	allGenres, 24
allGenres:
	.zero	24
	.globl	BoldWriterColor
	.data
	.align 4
	.type	BoldWriterColor, @object
	.size	BoldWriterColor, 4
BoldWriterColor:
	.long	-1
	.globl	Colors
	.section	.rodata
.LC4:
	.string	"Red"
.LC5:
	.string	"Green"
.LC6:
	.string	"Yellow"
.LC7:
	.string	"Blue"
.LC8:
	.string	"Purple"
.LC9:
	.string	"Cyan"
.LC10:
	.string	"White"
	.section	.data.rel.local
	.align 32
	.type	Colors, @object
	.size	Colors, 64
Colors:
	.quad	0
	.quad	.LC4
	.quad	.LC5
	.quad	.LC6
	.quad	.LC7
	.quad	.LC8
	.quad	.LC9
	.quad	.LC10
	.globl	writerType
	.align 16
	.type	writerType, @object
	.size	writerType, 24
writerType:
	.quad	typewriter
	.quad	epilepsy_typewriter
	.quad	bold_typewriter
	.globl	asciiArt
	.section	.rodata
.LC11:
	.string	"   ______________   "
.LC12:
	.string	"  /  __________  \\"
.LC13:
	.string	" |  | LITHIUM  |  | "
.LC14:
	.string	" |  |----------|  | "
.LC15:
	.string	" |  | TEETH    |  | "
.LC16:
	.string	" |  |__________|  |"
.LC17:
	.string	"|  _   ____   _  |"
.LC18:
	.string	" | (O) |____| (O) |"
.LC19:
	.string	"|  Coin [50c]    |"
.LC20:
	.string	" |________________|"
	.section	.data.rel.local
	.align 32
	.type	asciiArt, @object
	.size	asciiArt, 80
asciiArt:
	.quad	.LC11
	.quad	.LC12
	.quad	.LC13
	.quad	.LC14
	.quad	.LC15
	.quad	.LC16
	.quad	.LC17
	.quad	.LC18
	.quad	.LC19
	.quad	.LC20
	.globl	writerTypes
	.section	.rodata
.LC21:
	.string	"Pale White"
.LC22:
	.string	"RGB (seziure guranteed)"
.LC23:
	.string	"Bold X (pick your own color)"
	.section	.data.rel.local
	.align 16
	.type	writerTypes, @object
	.size	writerTypes, 24
writerTypes:
	.quad	.LC21
	.quad	.LC22
	.quad	.LC23
	.section	.rodata
	.align 8
.LC24:
	.string	"Getting the cutting edge version of lyrics and shit don't worry bruh\n\n"
.LC25:
	.string	"\033[H\033[J"
	.align 8
.LC26:
	.string	"If you see this you couldn't play shit...apparently duh\n"
.LC27:
	.base64	"G1tIG1tKG1swbQA="
	.text
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$256, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	call	fork@PLT
	movl	%eax, -248(%rbp)
	cmpl	$-1, -248(%rbp)
	jne	.L2
	movl	$-689, %eax
	jmp	.L13
.L2:
	cmpl	$0, -248(%rbp)
	jne	.L4
	call	errandBoy
.L4:
	movl	$0, %edi
	call	wait@PLT
	movl	$750000, %edi
	call	usleep@PLT
	call	SkyNet
	leaq	.LC25(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$250000, %edi
	call	usleep@PLT
	movl	$0, %edi
	call	time@PLT
	movl	%eax, %edi
	call	srand@PLT
	movq	stdout(%rip), %rax
	movl	$0, %ecx
	movl	$2, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	setvbuf@PLT
	leaq	-160(%rbp), %rax
	pxor	%xmm0, %xmm0
	movl	$152, %edx
	movl	%edx, %esi
	andl	$-64, %esi
	movl	$0, %ecx
.L5:
	movl	%ecx, %edx
	movaps	%xmm0, -160(%rbp,%rdx)
	movaps	%xmm0, -144(%rbp,%rdx)
	movaps	%xmm0, -128(%rbp,%rdx)
	movaps	%xmm0, -112(%rbp,%rdx)
	addl	$64, %ecx
	cmpl	%esi, %ecx
	jb	.L5
	movl	%ecx, %edx
	addq	%rdx, %rax
	movdqa	%xmm0, %xmm1
	movaps	%xmm1, (%rax)
	movq	%xmm0, %rdx
	movq	%rdx, 16(%rax)
	leaq	sigintHandler(%rip), %rax
	movq	%rax, -160(%rbp)
	leaq	-160(%rbp), %rax
	movl	$0, %edx
	movq	%rax, %rsi
	movl	$2, %edi
	call	sigaction@PLT
	movl	$0, -244(%rbp)
.L12:
	leaq	.LC25(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	-240(%rbp), %rax
	movq	%rax, %rdi
	call	genreSelection
	movl	-224(%rbp), %eax
	cmpl	$-1, %eax
	jne	.L7
	movl	$-13, %eax
	jmp	.L13
.L7:
	leaq	-208(%rbp), %rsi
	subq	$8, %rsp
	subq	$24, %rsp
	movq	%rsp, %rcx
	movq	-240(%rbp), %rax
	movq	-232(%rbp), %rdx
	movq	%rax, (%rcx)
	movq	%rdx, 8(%rcx)
	movq	-224(%rbp), %rax
	movq	%rax, 16(%rcx)
	movq	%rsi, %rdi
	call	songSelection
	addq	$32, %rsp
	movl	-176(%rbp), %edx
	movl	-224(%rbp), %eax
	cmpl	%eax, %edx
	jne	.L9
	movl	$-53, %eax
	jmp	.L13
.L9:
	call	fork@PLT
	movl	%eax, -244(%rbp)
	cmpl	$-1, -244(%rbp)
	jne	.L10
	movl	$-1, %eax
	jmp	.L13
.L10:
	cmpl	$0, -244(%rbp)
	jne	.L11
	movq	-192(%rbp), %rax
	movq	%rax, %rdi
	call	playShit
	leaq	.LC26(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %edi
	call	exit@PLT
.L11:
	movl	$3000000, %edi
	call	usleep@PLT
	movl	-176(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	writerType(%rip), %rax
	movq	(%rdx,%rax), %rcx
	movq	-184(%rbp), %rdx
	movq	-200(%rbp), %rax
	movq	%rdx, %xmm0
	movq	%rax, %rdi
	call	*%rcx
	movl	$0, %edi
	call	wait@PLT
	movl	$3000000, %edi
	call	usleep@PLT
	leaq	.LC27(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	jmp	.L12
.L13:
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L14
	call	__stack_chk_fail@PLT
.L14:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC28:
	.string	"Rickroll child fucked up and I can't return value in a void func"
.LC30:
	.base64	"G1swbRtbSBtbSgA="
.LC31:
	.base64	"G1syShtbSAA="
.LC32:
	.string	"\033[?25h"
	.text
	.globl	sigintHandler
	.type	sigintHandler, @function
sigintHandler:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	call	rand@PLT
	movl	%eax, %edx
	movl	%edx, %eax
	sarl	$31, %eax
	shrl	$30, %eax
	addl	%eax, %edx
	andl	$3, %edx
	subl	%eax, %edx
	movl	%edx, %eax
	addl	$1, %eax
	movl	%eax, -8(%rbp)
	cmpl	$3, -8(%rbp)
	jne	.L16
	leaq	.LC25(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$5000, %edi
	call	usleep@PLT
	call	fork@PLT
	movl	%eax, -4(%rbp)
	cmpl	$-1, -4(%rbp)
	jne	.L17
	leaq	.LC28(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movl	$-1987, %edi
	call	exit@PLT
.L17:
	cmpl	$0, -4(%rbp)
	jne	.L18
	movq	rickrollAudio(%rip), %rax
	movq	%rax, %rdi
	call	playShit
	jmp	.L16
.L18:
	movq	rickroll(%rip), %rax
	movq	.LC29(%rip), %rdx
	movq	%rdx, %xmm0
	movq	%rax, %rdi
	call	epilepsy_typewriter
	movl	$0, %edi
	call	wait@PLT
.L16:
	leaq	.LC30(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	.LC31(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	.LC32(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %edi
	call	exit@PLT
	.cfi_endproc
.LFE7:
	.size	sigintHandler, .-sigintHandler
	.section	.rodata
.LC33:
	.string	"\033[?25l"
.LC42:
	.string	"\033[38;2;%d;%d;%dm%c"
.LC45:
	.string	"\033[0m"
	.text
	.globl	epilepsy_typewriter
	.type	epilepsy_typewriter, @function
epilepsy_typewriter:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movq	%rdi, -72(%rbp)
	movsd	%xmm0, -80(%rbp)
	leaq	.LC33(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	.LC25(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$250000, %edi
	call	usleep@PLT
	movl	$0, -56(%rbp)
	movq	-72(%rbp), %rax
	movq	%rax, %rdi
	call	countPrintables
	movq	%rax, -8(%rbp)
	movsd	-80(%rbp), %xmm1
	movsd	.LC34(%rip), %xmm0
	mulsd	%xmm1, %xmm0
	cvttsd2sil	%xmm0, %eax
	movslq	%eax, %rdx
	imulq	$351843721, %rdx, %rdx
	shrq	$32, %rdx
	sarl	$13, %edx
	movl	%eax, %ecx
	sarl	$31, %ecx
	subl	%ecx, %edx
	movl	%edx, -52(%rbp)
	movl	-52(%rbp), %edx
	imull	$100000, %edx, %edx
	subl	%edx, %eax
	movl	%eax, -52(%rbp)
	movl	-52(%rbp), %eax
	movslq	%eax, %rdx
	imulq	$274877907, %rdx, %rdx
	shrq	$32, %rdx
	movl	%edx, %ecx
	sarl	$6, %ecx
	cltd
	subl	%edx, %ecx
	imull	$1000, %ecx, %edx
	subl	%edx, %eax
	movl	%eax, -48(%rbp)
	movsd	-80(%rbp), %xmm1
	movsd	.LC35(%rip), %xmm0
	mulsd	%xmm1, %xmm0
	movsd	%xmm0, -80(%rbp)
	movl	-4(%rbp), %eax
	testl	%eax, %eax
	jne	.L20
	movl	$1, -4(%rbp)
.L20:
	movl	-4(%rbp), %eax
	pxor	%xmm1, %xmm1
	cvtsi2sdl	%eax, %xmm1
	movsd	-80(%rbp), %xmm0
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -24(%rbp)
	movl	-8(%rbp), %eax
	pxor	%xmm1, %xmm1
	cvtsi2sdl	%eax, %xmm1
	movsd	-80(%rbp), %xmm0
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -16(%rbp)
	movl	-48(%rbp), %eax
	imull	$1000000, %eax, %eax
	movl	%eax, %edi
	call	usleep@PLT
	movq	$0, -32(%rbp)
	jmp	.L21
.L26:
	pxor	%xmm1, %xmm1
	cvtsi2sdq	-32(%rbp), %xmm1
	movsd	.LC36(%rip), %xmm0
	mulsd	%xmm0, %xmm1
	movsd	.LC37(%rip), %xmm0
	mulsd	%xmm0, %xmm1
	movq	%xmm1, %rax
	movq	%rax, %xmm0
	call	sin@PLT
	movsd	.LC38(%rip), %xmm1
	mulsd	%xmm0, %xmm1
	movsd	.LC39(%rip), %xmm0
	addsd	%xmm1, %xmm0
	cvttsd2sil	%xmm0, %eax
	movl	%eax, -44(%rbp)
	pxor	%xmm1, %xmm1
	cvtsi2sdq	-32(%rbp), %xmm1
	movsd	.LC36(%rip), %xmm0
	mulsd	%xmm0, %xmm1
	movsd	.LC37(%rip), %xmm0
	mulsd	%xmm0, %xmm1
	movsd	.LC40(%rip), %xmm0
	addsd	%xmm0, %xmm1
	movq	%xmm1, %rax
	movq	%rax, %xmm0
	call	sin@PLT
	movsd	.LC38(%rip), %xmm1
	mulsd	%xmm0, %xmm1
	movsd	.LC39(%rip), %xmm0
	addsd	%xmm1, %xmm0
	cvttsd2sil	%xmm0, %eax
	movl	%eax, -40(%rbp)
	pxor	%xmm1, %xmm1
	cvtsi2sdq	-32(%rbp), %xmm1
	movsd	.LC36(%rip), %xmm0
	mulsd	%xmm0, %xmm1
	movsd	.LC37(%rip), %xmm0
	mulsd	%xmm0, %xmm1
	movsd	.LC41(%rip), %xmm0
	addsd	%xmm0, %xmm1
	movq	%xmm1, %rax
	movq	%rax, %xmm0
	call	sin@PLT
	movsd	.LC38(%rip), %xmm1
	mulsd	%xmm0, %xmm1
	movsd	.LC39(%rip), %xmm0
	addsd	%xmm1, %xmm0
	cvttsd2sil	%xmm0, %eax
	movl	%eax, -36(%rbp)
	movq	-72(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %esi
	movl	-36(%rbp), %ecx
	movl	-40(%rbp), %edx
	movl	-44(%rbp), %eax
	leaq	.LC42(%rip), %rdi
	movl	%esi, %r8d
	movl	%eax, %esi
	movl	$0, %eax
	call	printf@PLT
	movq	stdout(%rip), %rax
	movq	%rax, %rdi
	call	fflush@PLT
	addl	$1, -56(%rbp)
	movq	-72(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$10, %al
	jne	.L22
	pxor	%xmm0, %xmm0
	cvtsi2sdl	-56(%rbp), %xmm0
	movapd	%xmm0, %xmm1
	mulsd	-16(%rbp), %xmm1
	movsd	-24(%rbp), %xmm0
	subsd	%xmm1, %xmm0
	pxor	%xmm1, %xmm1
	comisd	%xmm1, %xmm0
	jbe	.L28
	pxor	%xmm0, %xmm0
	cvtsi2sdl	-56(%rbp), %xmm0
	movapd	%xmm0, %xmm1
	mulsd	-16(%rbp), %xmm1
	movsd	-24(%rbp), %xmm0
	subsd	%xmm1, %xmm0
	cvttsd2siq	%xmm0, %rax
	movl	%eax, %edi
	call	usleep@PLT
	jmp	.L25
.L28:
	movl	$950000, %edi
	call	usleep@PLT
.L25:
	movl	$0, -56(%rbp)
.L22:
	movsd	-16(%rbp), %xmm0
	addsd	%xmm0, %xmm0
	movsd	.LC44(%rip), %xmm1
	divsd	%xmm1, %xmm0
	cvttsd2siq	%xmm0, %rax
	movl	%eax, %edi
	call	usleep@PLT
	addq	$1, -72(%rbp)
	addq	$1, -32(%rbp)
.L21:
	movq	-72(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L26
	leaq	.LC45(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$7500000, %edi
	call	usleep@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	epilepsy_typewriter, .-epilepsy_typewriter
	.section	.rodata
.LC46:
	.string	"\033[1;91m"
.LC47:
	.string	"\033[1;9%dm"
	.text
	.globl	bold_typewriter
	.type	bold_typewriter, @function
bold_typewriter:
.LFB9:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -56(%rbp)
	movsd	%xmm0, -64(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, %rdi
	call	countPrintables
	movq	%rax, -8(%rbp)
	movl	$0, -36(%rbp)
	movsd	-64(%rbp), %xmm1
	movsd	.LC34(%rip), %xmm0
	mulsd	%xmm1, %xmm0
	cvttsd2sil	%xmm0, %eax
	movslq	%eax, %rdx
	imulq	$351843721, %rdx, %rdx
	shrq	$32, %rdx
	sarl	$13, %edx
	movl	%eax, %ecx
	sarl	$31, %ecx
	subl	%ecx, %edx
	movl	%edx, -32(%rbp)
	movl	-32(%rbp), %edx
	imull	$100000, %edx, %edx
	subl	%edx, %eax
	movl	%eax, -32(%rbp)
	movl	-32(%rbp), %eax
	movslq	%eax, %rdx
	imulq	$274877907, %rdx, %rdx
	shrq	$32, %rdx
	movl	%edx, %ecx
	sarl	$6, %ecx
	cltd
	subl	%edx, %ecx
	imull	$1000, %ecx, %edx
	subl	%edx, %eax
	movl	%eax, -28(%rbp)
	movsd	-64(%rbp), %xmm1
	movsd	.LC35(%rip), %xmm0
	mulsd	%xmm1, %xmm0
	movsd	%xmm0, -64(%rbp)
	movl	-4(%rbp), %eax
	pxor	%xmm1, %xmm1
	cvtsi2sdl	%eax, %xmm1
	movsd	-64(%rbp), %xmm0
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -24(%rbp)
	movl	-8(%rbp), %eax
	pxor	%xmm1, %xmm1
	cvtsi2sdl	%eax, %xmm1
	movsd	-64(%rbp), %xmm0
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -16(%rbp)
	leaq	.LC46(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	.LC33(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	.LC25(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	BoldWriterColor(%rip), %eax
	leaq	.LC47(%rip), %rdx
	movl	%eax, %esi
	movq	%rdx, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	-28(%rbp), %eax
	imull	$1000000, %eax, %eax
	movl	%eax, %edi
	call	usleep@PLT
	jmp	.L30
.L36:
	addl	$1, -36(%rbp)
	movq	-56(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$10, %al
	jne	.L31
	pxor	%xmm0, %xmm0
	cvtsi2sdl	-36(%rbp), %xmm0
	movapd	%xmm0, %xmm1
	mulsd	-16(%rbp), %xmm1
	movsd	-24(%rbp), %xmm0
	subsd	%xmm1, %xmm0
	pxor	%xmm1, %xmm1
	comisd	%xmm1, %xmm0
	jbe	.L38
	pxor	%xmm0, %xmm0
	cvtsi2sdl	-36(%rbp), %xmm0
	movapd	%xmm0, %xmm1
	mulsd	-16(%rbp), %xmm1
	movsd	-24(%rbp), %xmm0
	subsd	%xmm1, %xmm0
	cvttsd2siq	%xmm0, %rax
	movl	%eax, %edi
	call	usleep@PLT
	jmp	.L34
.L38:
	movl	$1000000, %edi
	call	usleep@PLT
.L34:
	movq	-56(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %eax
	movl	%eax, %edi
	call	putchar@PLT
	movl	$0, -36(%rbp)
	jmp	.L35
.L31:
	movq	-56(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %eax
	movl	%eax, %edi
	call	putchar@PLT
	movsd	-16(%rbp), %xmm0
	addsd	%xmm0, %xmm0
	movsd	.LC44(%rip), %xmm1
	divsd	%xmm1, %xmm0
	cvttsd2siq	%xmm0, %rax
	movl	%eax, %edi
	call	usleep@PLT
.L35:
	addq	$1, -56(%rbp)
.L30:
	movq	-56(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L36
	leaq	.LC45(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$7500000, %edi
	call	usleep@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	bold_typewriter, .-bold_typewriter
	.globl	typewriter
	.type	typewriter, @function
typewriter:
.LFB10:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -56(%rbp)
	movsd	%xmm0, -64(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, %rdi
	call	countPrintables
	movq	%rax, -8(%rbp)
	movl	$0, -36(%rbp)
	movsd	-64(%rbp), %xmm1
	movsd	.LC34(%rip), %xmm0
	mulsd	%xmm1, %xmm0
	cvttsd2sil	%xmm0, %eax
	movslq	%eax, %rdx
	imulq	$351843721, %rdx, %rdx
	shrq	$32, %rdx
	sarl	$13, %edx
	movl	%eax, %ecx
	sarl	$31, %ecx
	subl	%ecx, %edx
	movl	%edx, -32(%rbp)
	movl	-32(%rbp), %edx
	imull	$100000, %edx, %edx
	subl	%edx, %eax
	movl	%eax, -32(%rbp)
	movl	-32(%rbp), %eax
	movslq	%eax, %rdx
	imulq	$274877907, %rdx, %rdx
	shrq	$32, %rdx
	movl	%edx, %ecx
	sarl	$6, %ecx
	cltd
	subl	%edx, %ecx
	imull	$1000, %ecx, %edx
	subl	%edx, %eax
	movl	%eax, -28(%rbp)
	movsd	-64(%rbp), %xmm1
	movsd	.LC35(%rip), %xmm0
	mulsd	%xmm1, %xmm0
	movsd	%xmm0, -64(%rbp)
	movl	-4(%rbp), %eax
	pxor	%xmm1, %xmm1
	cvtsi2sdl	%eax, %xmm1
	movsd	-64(%rbp), %xmm0
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -24(%rbp)
	movl	-8(%rbp), %eax
	pxor	%xmm1, %xmm1
	cvtsi2sdl	%eax, %xmm1
	movsd	-64(%rbp), %xmm0
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -16(%rbp)
	leaq	.LC33(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	.LC25(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	-28(%rbp), %eax
	imull	$1000000, %eax, %eax
	movl	%eax, %edi
	call	usleep@PLT
	jmp	.L40
.L46:
	addl	$1, -36(%rbp)
	movq	-56(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$10, %al
	jne	.L41
	pxor	%xmm0, %xmm0
	cvtsi2sdl	-36(%rbp), %xmm0
	movapd	%xmm0, %xmm1
	mulsd	-16(%rbp), %xmm1
	movsd	-24(%rbp), %xmm0
	subsd	%xmm1, %xmm0
	pxor	%xmm1, %xmm1
	comisd	%xmm1, %xmm0
	jbe	.L48
	pxor	%xmm0, %xmm0
	cvtsi2sdl	-36(%rbp), %xmm0
	movapd	%xmm0, %xmm1
	mulsd	-16(%rbp), %xmm1
	movsd	-24(%rbp), %xmm0
	subsd	%xmm1, %xmm0
	cvttsd2siq	%xmm0, %rax
	movl	%eax, %edi
	call	usleep@PLT
	jmp	.L44
.L48:
	movl	$1000000, %edi
	call	usleep@PLT
.L44:
	movq	-56(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %eax
	movl	%eax, %edi
	call	putchar@PLT
	movl	$0, -36(%rbp)
	jmp	.L45
.L41:
	movq	-56(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %eax
	movl	%eax, %edi
	call	putchar@PLT
	movsd	-16(%rbp), %xmm0
	addsd	%xmm0, %xmm0
	movsd	.LC44(%rip), %xmm1
	divsd	%xmm1, %xmm0
	cvttsd2siq	%xmm0, %rax
	movl	%eax, %edi
	call	usleep@PLT
.L45:
	addq	$1, -56(%rbp)
.L40:
	movq	-56(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L46
	movl	$7500000, %edi
	call	usleep@PLT
	leaq	.LC45(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	typewriter, .-typewriter
	.section	.rodata
.LC48:
	.string	"  \t\t\t%s\n"
.LC49:
	.string	"\n"
	.text
	.globl	asciiPrinter
	.type	asciiPrinter, @function
asciiPrinter:
.LFB11:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$0, -4(%rbp)
	jmp	.L50
.L51:
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	asciiArt(%rip), %rax
	movq	(%rdx,%rax), %rax
	leaq	.LC48(%rip), %rdx
	movq	%rax, %rsi
	movq	%rdx, %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -4(%rbp)
.L50:
	movl	-4(%rbp), %eax
	cmpl	$9, %eax
	jbe	.L51
	leaq	.LC49(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	asciiPrinter, .-asciiPrinter
	.globl	clearIBuffer
	.type	clearIBuffer, @function
clearIBuffer:
.LFB12:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	nop
.L54:
	call	getchar@PLT
	movl	%eax, -4(%rbp)
	cmpl	$10, -4(%rbp)
	je	.L53
	cmpl	$-1, -4(%rbp)
	jne	.L54
.L53:
	cmpl	$-1, -4(%rbp)
	jne	.L56
	movq	stdin(%rip), %rax
	movq	%rax, %rdi
	call	clearerr@PLT
.L56:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	clearIBuffer, .-clearIBuffer
	.section	.rodata
.LC50:
	.string	"nobuffer"
.LC51:
	.string	"-fflags"
.LC52:
	.string	"-autoexit"
.LC53:
	.string	"-nodisp"
.LC54:
	.string	"ffplay"
.LC55:
	.string	"8"
.LC56:
	.string	"-loglevel"
.LC57:
	.string	"experimental"
.LC58:
	.string	"-strict"
.LC59:
	.string	"0"
.LC60:
	.string	"-analyzeduration"
.LC61:
	.string	"32"
.LC62:
	.string	"-probesize"
.LC63:
	.string	"low_delay"
.LC64:
	.string	"-flags"
	.text
	.globl	playShit
	.type	playShit, @function
playShit:
.LFB13:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	leaq	.LC50(%rip), %r9
	leaq	.LC51(%rip), %r8
	leaq	.LC52(%rip), %rcx
	leaq	.LC53(%rip), %rdx
	leaq	.LC54(%rip), %rsi
	leaq	.LC54(%rip), %rax
	pushq	$0
	leaq	.LC55(%rip), %rdi
	pushq	%rdi
	leaq	.LC56(%rip), %rdi
	pushq	%rdi
	pushq	-24(%rbp)
	leaq	.LC57(%rip), %rdi
	pushq	%rdi
	leaq	.LC58(%rip), %rdi
	pushq	%rdi
	leaq	.LC59(%rip), %rdi
	pushq	%rdi
	leaq	.LC60(%rip), %rdi
	pushq	%rdi
	leaq	.LC61(%rip), %rdi
	pushq	%rdi
	leaq	.LC62(%rip), %rdi
	pushq	%rdi
	leaq	.LC63(%rip), %rdi
	pushq	%rdi
	leaq	.LC64(%rip), %rdi
	pushq	%rdi
	movq	%rax, %rdi
	movl	$0, %eax
	call	execlp@PLT
	addq	$96, %rsp
	movl	%eax, -4(%rbp)
	cmpl	$-1, -4(%rbp)
	jne	.L58
	movl	$-7, %eax
	jmp	.L59
.L58:
	movl	$0, %eax
.L59:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	playShit, .-playShit
	.globl	countPrintables
	.type	countPrintables, @function
countPrintables:
.LFB14:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L61
.L64:
	movq	-24(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$10, %al
	je	.L62
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -8(%rbp)
	jmp	.L63
.L62:
	movl	-4(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -4(%rbp)
.L63:
	addq	$1, -24(%rbp)
.L61:
	movq	-24(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L64
	movq	-8(%rbp), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE14:
	.size	countPrintables, .-countPrintables
	.section	.rodata
.LC65:
	.base64	"G1tIG1tKG1sxOzkxbQA="
.LC66:
	.string	"%2d)%s\n"
	.align 8
.LC67:
	.string	"\033[0m\nPick your poison bruh (1-%d): "
	.align 8
.LC68:
	.string	"Either pick a genre or Delta the fuck out bro"
	.text
	.globl	genreSelection
	.type	genreSelection, @function
genreSelection:
.LFB15:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -56(%rbp)
	leaq	.LC65(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, -44(%rbp)
	jmp	.L67
.L68:
	movl	-44(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	movq	%rax, %rdx
	leaq	allGenres(%rip), %rax
	movq	(%rdx,%rax), %rax
	movl	-44(%rbp), %edx
	leal	1(%rdx), %esi
	leaq	.LC66(%rip), %rcx
	movq	%rax, %rdx
	movq	%rcx, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$100000, %edi
	call	usleep@PLT
	addl	$1, -44(%rbp)
.L67:
	cmpl	$0, -44(%rbp)
	je	.L68
	leaq	.LC67(%rip), %rax
	movl	$1, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	call	fork@PLT
	movl	%eax, -40(%rbp)
	cmpl	$-1, -40(%rbp)
	jne	.L69
	movq	$0, -32(%rbp)
	movq	$0, -24(%rbp)
	movl	$-1, -16(%rbp)
	movq	-56(%rbp), %rcx
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rax, (%rcx)
	movq	%rdx, 8(%rcx)
	movq	-16(%rbp), %rax
	movq	%rax, 16(%rcx)
	jmp	.L66
.L69:
	cmpl	$0, -40(%rbp)
	jne	.L71
	movl	$60000000, %edi
	call	usleep@PLT
	call	getppid@PLT
	movl	$15, %esi
	movl	%eax, %edi
	call	kill@PLT
	movq	roast(%rip), %rax
	movq	%rax, %rdi
	call	playShit
.L71:
	movl	$1, %esi
	movl	$1, %edi
	call	safe_scanf
	movl	%eax, -36(%rbp)
	cmpl	$-1, -36(%rbp)
	je	.L72
	cmpl	$1368953, -36(%rbp)
	jne	.L73
.L72:
	movq	$0, -32(%rbp)
	movq	$0, -24(%rbp)
	movl	$-1, -16(%rbp)
	leaq	.LC68(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movl	-40(%rbp), %eax
	movl	$15, %esi
	movl	%eax, %edi
	call	kill@PLT
	movq	-56(%rbp), %rcx
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rax, (%rcx)
	movq	%rdx, 8(%rcx)
	movq	-16(%rbp), %rax
	movq	%rax, 16(%rcx)
	jmp	.L66
.L73:
	movl	-36(%rbp), %eax
	subl	$1, %eax
	movq	-56(%rbp), %rcx
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	movq	%rax, %rdi
	leaq	allGenres(%rip), %rsi
	movq	(%rdi,%rsi), %rax
	movq	8(%rdi,%rsi), %rdx
	movq	%rax, (%rcx)
	movq	%rdx, 8(%rcx)
	movq	16(%rdi,%rsi), %rax
	movq	%rax, 16(%rcx)
.L66:
	movq	-56(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE15:
	.size	genreSelection, .-genreSelection
	.section	.rodata
.LC69:
	.base64	"G1tIG1tKG1sxOzk1bQA="
	.align 8
.LC70:
	.string	"\033[0mWhat you wanna listen bruh (1-%d):"
	.align 8
.LC71:
	.string	"How hard can it be to pick a fucking number on a screen...Just...Delta"
.LC72:
	.string	"Fork got spooned"
.LC73:
	.string	"This one's personal..."
	.align 8
.LC74:
	.string	"Playing: Mea Lux - Ad Lucem Meum\n\033[0m"
.LC75:
	.string	"%d)%s\n"
.LC76:
	.string	"Select a writer bruh: "
.LC77:
	.string	" %d"
	.align 8
.LC78:
	.string	"Bro just learn to count up to %d...Punishment:RGB Seziures!!!!\n"
	.text
	.globl	songSelection
	.type	songSelection, @function
songSelection:
.LFB16:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$56, %rsp
	.cfi_offset 3, -24
	movq	%rdi, -56(%rbp)
	leaq	.LC69(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, -32(%rbp)
	jmp	.L75
.L76:
	movq	24(%rbp), %rcx
	movl	-32(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	(%rax), %rax
	movl	-32(%rbp), %edx
	leal	1(%rdx), %esi
	leaq	.LC66(%rip), %rcx
	movq	%rax, %rdx
	movq	%rcx, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$130000, %edi
	call	usleep@PLT
	addl	$1, -32(%rbp)
.L75:
	movl	32(%rbp), %eax
	cmpl	%eax, -32(%rbp)
	jl	.L76
	movl	32(%rbp), %eax
	leaq	.LC70(%rip), %rdx
	movl	%eax, %esi
	movq	%rdx, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	32(%rbp), %eax
	movl	%eax, %esi
	movl	$1, %edi
	call	safe_scanf
	movl	%eax, -28(%rbp)
	cmpl	$-1, -28(%rbp)
	jne	.L77
	leaq	.LC71(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movq	24(%rbp), %rcx
	movl	32(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	leaq	(%rcx,%rax), %rdx
	movq	-56(%rbp), %rax
	movq	(%rdx), %rcx
	movq	8(%rdx), %rbx
	movq	%rcx, (%rax)
	movq	%rbx, 8(%rax)
	movq	16(%rdx), %rcx
	movq	24(%rdx), %rbx
	movq	%rcx, 16(%rax)
	movq	%rbx, 24(%rax)
	movq	32(%rdx), %rdx
	movq	%rdx, 32(%rax)
	jmp	.L78
.L77:
	cmpl	$1368953, -28(%rbp)
	jne	.L79
	call	fork@PLT
	movl	%eax, -24(%rbp)
	cmpl	$-1, -24(%rbp)
	jne	.L80
	leaq	.LC72(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %edi
	call	exit@PLT
.L80:
	leaq	.LC69(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	.LC73(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$1500000, %edi
	call	usleep@PLT
	leaq	.LC25(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	.LC74(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	meaLuxAudio(%rip), %rax
	movq	%rax, %rdi
	call	playShit
.L79:
	movl	$3, -20(%rbp)
	leaq	.LC65(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movw	$0, -34(%rbp)
	jmp	.L81
.L82:
	movswl	-34(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	writerTypes(%rip), %rax
	movq	(%rdx,%rax), %rax
	movswl	-34(%rbp), %edx
	leal	1(%rdx), %esi
	leaq	.LC75(%rip), %rcx
	movq	%rax, %rdx
	movq	%rcx, %rdi
	movl	$0, %eax
	call	printf@PLT
	movzwl	-34(%rbp), %eax
	addl	$1, %eax
	movw	%ax, -34(%rbp)
.L81:
	movswl	-34(%rbp), %eax
	cmpl	%eax, -20(%rbp)
	jg	.L82
	leaq	.LC45(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	.LC76(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	24(%rbp), %rcx
	movl	-28(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	subq	$40, %rax
	addq	%rcx, %rax
	leaq	32(%rax), %rdx
	leaq	.LC77(%rip), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	__isoc23_scanf@PLT
	cmpl	$1, %eax
	jne	.L83
	movq	24(%rbp), %rcx
	movl	-28(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	subq	$40, %rax
	addq	%rcx, %rax
	movl	32(%rax), %eax
	testl	%eax, %eax
	jle	.L83
	movq	24(%rbp), %rcx
	movl	-28(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	subq	$40, %rax
	addq	%rcx, %rax
	movl	32(%rax), %eax
	cmpl	%eax, -20(%rbp)
	jge	.L84
.L83:
	movl	-20(%rbp), %eax
	leaq	.LC78(%rip), %rdx
	movl	%eax, %esi
	movq	%rdx, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	24(%rbp), %rcx
	movl	-28(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	subq	$40, %rax
	addq	%rcx, %rax
	movl	$2, 32(%rax)
.L84:
	movq	24(%rbp), %rcx
	movl	-28(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	subq	$40, %rax
	addq	%rcx, %rax
	movl	32(%rax), %ecx
	movq	24(%rbp), %rsi
	movl	-28(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	subq	$40, %rax
	addq	%rsi, %rax
	leal	-1(%rcx), %edx
	movl	%edx, 32(%rax)
	movq	24(%rbp), %rcx
	movl	-28(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	subq	$40, %rax
	addq	%rcx, %rax
	movl	32(%rax), %eax
	cmpl	$2, %eax
	jne	.L85
	call	colorPicker
.L85:
	movq	24(%rbp), %rcx
	movl	-28(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	subq	$40, %rax
	leaq	(%rcx,%rax), %rdx
	movq	-56(%rbp), %rax
	movq	(%rdx), %rcx
	movq	8(%rdx), %rbx
	movq	%rcx, (%rax)
	movq	%rbx, 8(%rax)
	movq	16(%rdx), %rcx
	movq	24(%rdx), %rbx
	movq	%rcx, 16(%rax)
	movq	%rbx, 24(%rax)
	movq	32(%rdx), %rdx
	movq	%rdx, 32(%rax)
.L78:
	movq	-56(%rbp), %rax
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16:
	.size	songSelection, .-songSelection
	.section	.rodata
.LC79:
	.string	"./src/songdata.h"
	.align 8
.LC80:
	.string	"https://drive.google.com/uc?export=download&id=1rrCXmmoeMtXCK-0lUjjKZTSBRmMuTuOM"
.LC81:
	.string	"-o"
.LC82:
	.string	"-S"
.LC83:
	.string	"-J"
.LC84:
	.string	"-L"
.LC85:
	.string	"curl"
.LC86:
	.string	"curl exec fucked up hard"
	.text
	.globl	errandBoy
	.type	errandBoy, @function
errandBoy:
.LFB17:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	leaq	.LC79(%rip), %rax
	movq	%rax, -16(%rbp)
	leaq	.LC80(%rip), %rax
	movq	%rax, -8(%rbp)
	leaq	.LC81(%rip), %r8
	leaq	.LC82(%rip), %rdi
	leaq	.LC83(%rip), %rcx
	leaq	.LC84(%rip), %rdx
	leaq	.LC85(%rip), %rsi
	leaq	.LC85(%rip), %rax
	subq	$8, %rsp
	pushq	$0
	pushq	-8(%rbp)
	pushq	-16(%rbp)
	movq	%r8, %r9
	movq	%rdi, %r8
	movq	%rax, %rdi
	movl	$0, %eax
	call	execlp@PLT
	addq	$32, %rsp
	leaq	.LC86(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %edi
	call	exit@PLT
	.cfi_endproc
.LFE17:
	.size	errandBoy, .-errandBoy
	.section	.rodata
.LC87:
	.string	"Damn...not today sentient AIs"
.LC88:
	.string	"-lm"
.LC89:
	.string	"ShittyJukeBox"
.LC90:
	.string	"ShittyJukeBox.c"
.LC91:
	.string	"cc"
	.align 8
.LC92:
	.string	"Damn bro you wander in C repos and don't have cc shame on you...Delta\n"
.LC93:
	.string	"./ShittyJukeBox"
	.text
	.globl	SkyNet
	.type	SkyNet, @function
SkyNet:
.LFB18:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	call	fork@PLT
	movl	%eax, -4(%rbp)
	cmpl	$-1, -4(%rbp)
	jne	.L88
	leaq	.LC87(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movl	$1, %edi
	call	exit@PLT
.L88:
	cmpl	$0, -4(%rbp)
	jne	.L89
	call	fork@PLT
	movl	%eax, -4(%rbp)
	leaq	.LC88(%rip), %r8
	leaq	.LC89(%rip), %rdi
	leaq	.LC81(%rip), %rcx
	leaq	.LC90(%rip), %rdx
	leaq	.LC91(%rip), %rsi
	leaq	.LC91(%rip), %rax
	movq	%r8, %r9
	movq	%rdi, %r8
	movq	%rax, %rdi
	movl	$0, %eax
	call	execlp@PLT
	leaq	.LC92(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %edi
	call	exit@PLT
.L89:
	movl	$0, %edi
	call	wait@PLT
	movl	$1500000, %edi
	call	usleep@PLT
	leaq	.LC89(%rip), %rcx
	leaq	.LC93(%rip), %rax
	movl	$0, %edx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	execl@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE18:
	.size	SkyNet, .-SkyNet
	.section	.rodata
.LC94:
	.string	"Pick your color: "
.LC95:
	.string	"%d"
.LC96:
	.string	"You'll get red bitch"
	.text
	.globl	colorPicker
	.type	colorPicker, @function
colorPicker:
.LFB19:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	BoldWriterColor(%rip), %eax
	cmpl	$-1, %eax
	jne	.L95
	leaq	.LC46(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$1, -4(%rbp)
	jmp	.L92
.L93:
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	Colors(%rip), %rax
	movq	(%rdx,%rax), %rdx
	movl	-4(%rbp), %eax
	leaq	.LC75(%rip), %rcx
	movl	%eax, %esi
	movq	%rcx, %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -4(%rbp)
.L92:
	movl	-4(%rbp), %eax
	cmpl	$7, %eax
	jbe	.L93
	leaq	.LC45(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	.LC94(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	BoldWriterColor(%rip), %rdx
	leaq	.LC95(%rip), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	__isoc23_scanf@PLT
	cmpl	$1, %eax
	jne	.L94
	movl	BoldWriterColor(%rip), %eax
	testl	%eax, %eax
	jle	.L94
	movl	BoldWriterColor(%rip), %eax
	cmpl	$7, %eax
	jbe	.L95
.L94:
	leaq	.LC96(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$1, BoldWriterColor(%rip)
.L95:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE19:
	.size	colorPicker, .-colorPicker
	.globl	safe_scanf
	.type	safe_scanf, @function
safe_scanf:
.LFB20:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$0, -12(%rbp)
	leaq	-12(%rbp), %rax
	leaq	.LC77(%rip), %rdx
	movq	%rax, %rsi
	movq	%rdx, %rdi
	movl	$0, %eax
	call	__isoc23_scanf@PLT
	cmpl	$1, %eax
	jne	.L97
	movl	-12(%rbp), %eax
	cmpl	%eax, -20(%rbp)
	jg	.L97
	movl	-12(%rbp), %eax
	cmpl	%eax, -24(%rbp)
	jge	.L98
	movl	-12(%rbp), %eax
	cmpl	$1368953, %eax
	je	.L98
.L97:
	call	clearIBuffer
	movl	$-1, %eax
	jmp	.L100
.L98:
	movl	-12(%rbp), %eax
.L100:
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L101
	call	__stack_chk_fail@PLT
.L101:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE20:
	.size	safe_scanf, .-safe_scanf
	.section	.rodata
	.align 8
.LC29:
	.long	-967570232
	.long	1074433983
	.align 8
.LC34:
	.long	0
	.long	1090021888
	.align 8
.LC35:
	.long	0
	.long	1099734072
	.align 8
.LC36:
	.long	858993459
	.long	1071854387
	.align 8
.LC37:
	.long	-1717986918
	.long	1070176665
	.align 8
.LC38:
	.long	0
	.long	1080016896
	.align 8
.LC39:
	.long	0
	.long	1080033280
	.align 8
.LC40:
	.long	0
	.long	1073741824
	.align 8
.LC41:
	.long	0
	.long	1074790400
	.align 8
.LC44:
	.long	0
	.long	1074266112
	.ident	"GCC: (GNU) 16.1.1 20260430"
	.section	.note.GNU-stack,"",@progbits
