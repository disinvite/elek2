ideal
model large, c
locals @@

dataseg

    extrn evt_buf:far
    extrn evt_xpos:far
    extrn evt_ypos:far
    extrn evt_write_ptr:word
    extrn evt_read_ptr:word

codeseg

    saved_ds dw ?
    mouse_was_init dw ?
    saved_mask dw ?
    saved_interrupt dd ?

public Mouse_Interrupt
public Mouse_Init
public Mouse_Shutdown
;public Mouse_PumpEvents

proc Mouse_Interrupt
uses ds

    mov ds, [cs:saved_ds]

    ; Grab index, double to word size
    mov bx, [evt_write_ptr]
    shl bx, 1

    ; Set mouse event mask, x/y position
    mov [word bx + evt_buf],  ax
    mov [word bx + evt_xpos], cx
    mov [word bx + evt_ypos], dx

    inc [evt_write_ptr]
    and [evt_write_ptr], 15

    mov ax, [evt_read_ptr]
    cmp ax, [evt_write_ptr]
    jne @@done

    ; Overflow detected, delete oldest event
    inc [evt_read_ptr]
    and [evt_read_ptr], 15

@@done:
    ret

endp

proc Mouse_Init
    ; Save main DS so we can restore it inside the interrupt function.
    mov [cs:saved_ds], ds

    mov ax, [cs:mouse_was_init]
    or ax, ax
    jnz @@done

    ; Setup pointer to new interrupt function
    mov ax, seg    Mouse_Interrupt
    mov es, ax
    mov dx, offset Mouse_Interrupt

    ; AX = 14h : Swap interrupt function. Set ours and save the old one.
    ; CX = 255 : Mask all interrupt types
    mov ax, 14h
    mov cx, 0ffh
    int 33h

    ; Save old interrupt and mask
    mov [word high cs:saved_interrupt], es
    mov [word low  cs:saved_interrupt], dx
    mov [cs:saved_mask], cx

@@done:
    ret

endp

proc Mouse_Shutdown

    mov ax, [cs:mouse_was_init]
    or ax, ax
    jz @@done

    ; Prepare to restore previous interrupt/mask if there was one.
    mov es, [word high cs:saved_interrupt]
    mov dx, [word low  cs:saved_interrupt]

    ; AX = 0ch : Set interrupt function.
    mov ax, 0ch
    mov cx, [cs:saved_mask]
    int 33h

@@done:
    ret
endp

; proc Mouse_PumpEvents
;     ; Grab index, double to word size
;     mov bx, [evt_read_ptr]
;     shl bx, 1
; 
;     mov ax, [word bx + evt_buf]
;     mov cx, [word bx + evt_xpos]
;     mov dx, [word bx + evt_ypos]
; 
;     mov bx, ax
;     and bx, 1
;     jz @@no_xy
; 
; @@no_xy:
; 
; endp

end
