.arm
.section .startup, "ax"

_entry:
	mov r1, sp
	ldr r2, =gdb_mainloop
	ldr lr, =0
	ldr sp, =0xFFFC
	mov pc, r2
