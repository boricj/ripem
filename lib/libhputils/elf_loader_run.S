.arm
.section .text

.globl run_elf

run_elf:
	/* Flush DCache. */
_dcache_loop:
	mrc p15, 0, r15, c7, c14, 3
	bne _dcache_loop

	/* Clear caches. */
	mov r3, #0
	mcr p15, 0, r3, c7, c7, 0

	mov sp, r1
	mov pc, r2
