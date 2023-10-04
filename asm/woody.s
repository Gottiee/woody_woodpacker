section .text
	global _start

_start:
	mov r11, 0xffffffff 	; .text offset
	mov r12, 0xffffffff 	; .text size
	mov r13, 0xffffffff 	; new entry point

	lea rdi, [rel _start]
	sub rdi, r13
	add rdi, r11

	mov r10, rdi 			; save _start+0
	and rdi, -0x1000 		; 4096  -> point to _init+0
	neg rdi 				; calcul .init size
	add r10, rdi 
	neg rdi
	mov r13, r12
	add r13, r10			; .textsize + .init (size)

mprotect:
	mov rax, 10
	mov rsi, r13
	mov rdx, 7
	syscall 				; mprotect(rdi, rsi, rdx) => (address .inti, size of text + init, PROT RWX)

XORcipher:
	add rdi, r10            ; .init + r10 = _start (start of .text section)
							; r12 .text size
	lea rsi, [rel key]		; key address
	sub rsi, 1
	mov r14, 10				; size of key
	xor r8, r8

loop: 
	cmp r8, r12
	jge write				; r8 >= .text_size
	mov r9, r8
	xor rax, rax;
	xor rdx, rdx;
	mov rax, r9
	div r14
	mov bl, [rsi + rdx]
	cmp r8, 0
	je wtf
	xor BYTE [rdi + r8], bl
	inc r8
	jmp loop

wtf:
	xor BYTE [rdi], bl
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