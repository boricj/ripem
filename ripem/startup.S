.arm
.section .startup, "ax"

.extern _rel_start
.extern _rel_end

_entry:
#
# Relocate from DRAM to SRAM
#
	ldr r2, =0x2000
	ldr r3, =_rel_start
	ldr r4, =_rel_end
	add r3, #0x30000000
	add r4, #0x30000000

_copy_loop:
	ldr r1, [r3], #4
	str r1, [r2], #4
	cmp r3, r4
	bne _copy_loop

	mov r1, sp
	ldr r2, =main
	ldr lr, =0
	ldr sp, =0xFFFC
	mov pc, r2
