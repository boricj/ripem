.arm
.section .startup, "ax"

.extern _rel_start
.extern _rel_end
.extern _bss_start
.extern _bss_end

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

#
# Clear BSS
#
	ldr r1, =_bss_start
	ldr r2, =_bss_end
	mov r3, #0

_clear_bss_loop:
	str r3, [r1], #4
	cmp r1, r2
	blo _clear_bss_loop

	mov r1, sp
	ldr r2, =main
	ldr lr, =0
	ldr sp, =0xFFFC
	mov pc, r2
