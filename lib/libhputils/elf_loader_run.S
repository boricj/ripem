.arm
.section .text

.globl run_elf

run_elf:
	/* Flush and invalidate DCache. */
_dcache_loop:
	mrc p15, 0, r15, c7, c14, 3
	bne _dcache_loop

	/* Invalidate ICache. */
	mcr p15, 0, r0, c7, c5, 0

	mov sp, r1
	mov pc, r2
