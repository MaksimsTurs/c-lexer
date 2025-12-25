section .text

global main
extern exit
extern putchar

main:
push rbp
mov rbp, rsp
sub rsp, 16
mov [rbp - 8], 0
mov [rbp - 16], 65
mov rcx, 65
call putchar
mov rcx, 0
call exit
pop rbp
add rsp, 16
