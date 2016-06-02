; ARM assembly program to set and clear GPIO port 16 at regular time intervals.
;
; r0 => Memory address for accesing GPIO pins 10-19.
; r1 => Memory address to clear the pins.
; r2 => Memory address to set the pins.
; r3 => To set and clear bit 16 for pin 16.
; r4 => Loop variable.

	ldr r0, =0x20200004	; Memory address of GPIO pins 10-19
	ldr r1, =0x20200028	; Memory address to clear the pins
	ldr r2, =0x2020001C	; Memory address to set the pins

	mov r3, #1		; r3 = 1
	lsl r3, #18 		; lsl by 18(sets bit for pin 16)
	str r3, [r0]		; Accessed Pin 16

turn_on:
	mov r3, #1		; r3 = 1
	lsl r3, #16		; lsl by 16(sets pin 16 at memory address-r2)
	str r3, [r2]		; Pin 16 is set

delay1:
	mov r4, #0		; r4 = 0
next1:
	cmp r4, #0x1000000	; Compare r4 and 0x1000000 for the delay.
	bge turn_off		; If greater or equal, jump to turn_off.
	add r4, r4, #1		; r4++
	bal next1		; Next iteration of delay1.

turn_off:
	mov r3, #1		; r3 = 1
	lsl r3, #16		; lsl by 16(clears the bit for pin 16)
	str r3, [r1]		; Cleared pin 16.

delay2:
	mov r4, #0		; r4 = 0
next2:
	cmp r4, #0x1000000	; Compare r4 and 0x1000000 for the delay.
	bge turn_on		; If greater or equal, jump to turn_on.
	add r4, r4, #1		; r4++
	bal next2		; Next iteration of delay2.
