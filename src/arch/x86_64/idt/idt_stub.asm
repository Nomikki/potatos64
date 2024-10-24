[bits 64]
[extern interrupt_dispatch]


%macro interrupt_routine 1
[global interrupt_service_routine_%1]
interrupt_service_routine_%1:
    ;cli
    push 0 ; No error code
    push %1 ; Vector number
    save_context
    mov rdi, rsp
    ; cld 
    call interrupt_dispatch
    mov rsp, rax
    restore_context
    add rsp, 16
    iretq
%endmacro

%macro interrupt_err_routine 1
[global interrupt_service_routine_err_%1]
interrupt_service_routine_err_%1:
    ;cli
    ; push 0 ; error code
    push %1 ; Vector number
    save_context
    mov rdi, rsp
    ; cld
    call interrupt_dispatch
    mov rsp, rax
    restore_context
    add rsp, 16
    iretq
%endmacro


%macro save_context 0
    push rax
    push rbx
    push rcx
    push rdx

    push rbp
    push rsi
    push rdi

    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro restore_context 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8

    pop rdi
    pop rsi
    pop rbp

    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

interrupt_routine 0 
interrupt_routine 1 
interrupt_routine 2 
interrupt_routine 3 
interrupt_routine 4 
interrupt_routine 5 
interrupt_routine 6 
interrupt_routine 7 
interrupt_err_routine 8 
interrupt_routine 9 
interrupt_err_routine 10 
interrupt_err_routine 11 
interrupt_err_routine 12 
interrupt_err_routine 13 
interrupt_err_routine 14 
interrupt_routine 15 
interrupt_routine 16 
interrupt_err_routine 17 
interrupt_routine 18 
interrupt_routine 32 
interrupt_routine 33 
interrupt_routine 34 
interrupt_routine 255 

