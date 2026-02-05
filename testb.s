mov eax, 0x0A4948
push rax

mov eax, 1
mov edi, 1
mov rsi, rsp
mov edx, 3
syscall

mov eax, 60
mov edi, 0
syscall