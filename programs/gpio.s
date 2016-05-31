ldr r0, =0x20200004             ; Memory address of GPIO pins 10-19
ldr r1, =0x20200028             ; Memory address for clearing the pins
ldr r2, =0x2020001C             ; Memory address for turning on the pins

mov r3, #1                      ; r3 = 1
lsl r3, #18                     ; Logical shift left by 18(last bit of pin 16 at memory address from r0) bits on r3
str r3, [r0]                    ; Pin 16 set to output pin

turn_on:
        mov r3, #1              ; r3 = 1
        lsl r3, #16             ; Logical shift left by 16(sets the bit for pin 16 in the "turning on" memory address-r2)
        str r3, [r2]            ; Pin 16 is turned on

delay1:
        mov r4, #0              ; r4 = 0
next1:
        cmp r4, #0x1000000      ; Compares r4 with a really big value in order to delay the pin and sets the flags
        bge turn_off            ; If r4 > 0x1000000, jump to turning the led off
        add r4, r4, #1          ; Increment the value held by r4
        bal next1               ; Jump to next iteration of delay1

turn_off:
        mov r3, #1              ; r3 = 1
        lsl r3, #16             ; Logical shift left by 16(sets the bit for pin 16 in the clearing memory address-r1)
        str r3, [r1]            ; Pin 16 is turned off

delay2:
        mov r4, #0              ; r4 = 0
next2:
        cmp r4, #0x1000000      ; Compares r4 with a really big value in order to delay the pin and sets the flags
        bge turn_on             ; If r4 > 0x1000000, jump to turning the led on
        add r4, r4, #1          ; Increment the value held by r4
        bal next2               ; Jump to next iteration of delay2
