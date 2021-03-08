SECTION .text
main:
	mov edx, esp
	mov eax, 0x3
	push eax
	mov ecx, [edx]
	pop eax
