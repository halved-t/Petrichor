; we loove asm

bits 64

global gdt_flush
global tss_flush

; void gdt_flush(gdtr_t *gdtr)
; rdi = pointer to GDTR
gdt_flush:
    lgdt [rdi]

    ; Reload data segments
    mov ax, 0x10        ; GDT_KERNEL_DATA
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far return to reload CS with GDT_KERNEL_CODE (0x08)
    ; Push CS:RIP onto the stack and use retfq
    pop rdi             ; save return address
    push qword 0x08     ; new CS
    push rdi            ; return RIP
    retfq

; void tss_flush(uint16_t selector)
; rdi = TSS selector
tss_flush:
    mov ax, di
    ltr ax
    ret