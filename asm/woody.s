section .text
	global _start

_start:
	mov r11, 0xffffffff 	; .text offset
	mov r12, 0xffffffff 	; .text size
	mov r13, 0xffffffff 	; new entry point

	lea rdi, [rel _start]
	sub rdi, r13
	add rdi, r11

	mov r10, rdi
	and rdi, -0x1000 ; 4096 
	neg rdi
	add r10, rdi
	neg rdi
	add r12, r10

mprotect:
	mov rax, 10
	mov rsi, r12
	mov rdx, 7
	syscall 				; mprotect(rdi, rsi, rdx) => (address .text, size of text sect, PROT RWX)
	add rdi, r10

XORcipher:
							; rdi store .text address to overwrite
							; r12 .text size
	lea rsi, [rel key]		; key address
	sub rsi, 1				; ? wtf
	mov r14, 10				; size of key
	xor r8, r8
	xor rcx, rcx

loop: 
	cmp r8, r12
	jge write				; r8 >= .text_size
	; mov cl , [rdi + r8]
	mov r9, r8
	xor rax, rax;
	xor rdx, rdx;
	mov rax, r9
	div r14
	mov bl, [rsi + rdx]
	xor [rdi + r8], bl
	inc r8
	jmp loop

write:
	mov rax, 1
	mov rdi, 1
	lea rsi,[rel msg]
	mov rdx, msg_end - msg
	syscall
	jmp end

align 8
	msg db "....WOODY....", 0x0a, 0
	msg_end db 0x0
	key db "kkkkkkkkkk", 0

end:
	xor rax, rax
	xor rdi, rdi
	xor rdx, rdx
	xor rsi, rsi
	jmp 0xffffffff