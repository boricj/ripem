.arm
.section .text

.globl gdb_command_continue

gdb_command_continue:
	/* Flush DCache. */
_dcache_loop:
	mrc p15, 0, r15, c7, c14, 3
	bne _dcache_loop

	/* Clear caches. */
	mov r3, #0
	mcr p15, 0, r3, c7, c7, 0

	ldr r1, =gdb_regs

	ldr r0, [r1]
	ldr r13, [r1, #52]
	mov r14, #0
	ldr r15, [r1, #60]
