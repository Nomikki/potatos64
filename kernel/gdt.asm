section .data
align 8
gdt_start:
  dq 0x0000000000000000      ; Null segment
  dq 0x00CF9A000000FFFF      ; Code segment: base=0, limit=0xFFFFF, access=0x9A, granularity=0xCF
  dq 0x00CF92000000FFFF      ; Data segment: base=0, limit=0xFFFFF, access=0x92, granularity=0xCF
gdt_end:

gdt_descriptor:
  dw gdt_end - gdt_start - 1 ; Size of GDT
  dd gdt_start               ; Starting address of GDT

section .text
global load_gdt
global test_pmode


load_gdt:
  lgdt[gdt_descriptor]       ; Load GDT
  mov ax, 0x10               ; Load data segment (offset 0x10 gdt)
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  jmp 0x08:.flush_code       ; Load code segment (offset 0x08 gdt)


.flush_code:
  ret


test_pmode:
    mov eax, cr0      
    test eax, 1       
    jz .not_protected  ; 
    
    mov eax, 1
    ret

.not_protected:
    mov eax, 0
    ret

