.arm
.section .text

.globl gdb_command_continue

gdb_command_continue:
	/* Flush and invalidate DCache. */
_dcache_loop:
	mrc p15, 0, r15, c7, c14, 3
	bne _dcache_loop

	/* Invalidate ICache. */
	mcr p15, 0, r0, c7, c5, 0

	ldr r1, =gdb_regs

	ldr r0, [r1]
	ldr r13, [r1, #52]
	mov r14, #0
	ldr r15, [r1, #60]
