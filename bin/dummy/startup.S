.arm
.section .startup, "ax"

_entry:
	ldr r1, =main
	mov pc, r1
