bits 64

extern isr_dispatch   ; defined in isr.c

global idt_stub_table ; table of 256 stub addresses, used by idt.c

%macro ISR_NOERR 1
isr_stub_%1:
    push qword 0      ; dummy error code
    push qword %1     ; vector number
    jmp isr_common
%endmacro

%macro ISR_ERR 1
isr_stub_%1:
    push qword %1     ; vector number (error code already on stack)
    jmp isr_common
%endmacro

; Exceptions that push an error code: 8,10,11,12,13,14,17,21,29,30
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_ERR   21
%assign i 22
%rep 234
ISR_NOERR i
%assign i i+1
%endrep

isr_common:
    ; Save general-purpose registers (reverse order of interrupt_frame_t)
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; rdi = pointer to interrupt_frame_t (first arg to isr_dispatch)
    mov rdi, rsp
    call isr_dispatch

    ; Restore
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16       ; pop vector + error_code
    iretq

section .data
idt_stub_table:
%assign i 0
%rep 256
    dq isr_stub_%+i
%assign i i+1
%endrep