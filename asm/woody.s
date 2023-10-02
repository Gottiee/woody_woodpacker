section .text
	global _start

_start:
	mov rax, 1
	mov rdi, 1
	lea rsi,[rel msg]
	mov rdx, msg_end - msg
	syscall
	jmp end

align 8
	msg db "....WOODY....", 0x0a, 0
	msg_end db 0x0

end:
	xor rax, rax
	xor rdi, rdi
	xor rdx, rdx
	xor rsi, rsi
	jmp 0xffffffff