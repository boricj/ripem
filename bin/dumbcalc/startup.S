.arm
.section .startup, "ax"

_entry:
	ldr r1, =main
	ldr sp,=0x31fffff0
	mov pc, r1
