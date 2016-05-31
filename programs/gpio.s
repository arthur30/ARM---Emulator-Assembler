ldr r0, =0x20200004
ldr r1, =0x20200028
ldr r2, =0x2020001C

mov r3, #1
lsl r3, #18
str r3, [r0]

turn_on:
  mov r3, #1
  lsl r3, #16
  str r3, [r2]

delay1:
  mov r4, #0
next1:
  cmp r4, #0x1000000
  bge turn_off
  add r4, r4, #1
  bal next1

turn_off:
  mov r3, #1
  lsl r3, #16
  str r3, [r1]

delay2:
  mov r4, #0
next2:
  cmp r4, #0x1000000
  bge turn_on
  add r4, r4, #1
  bal next2
