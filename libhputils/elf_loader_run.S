.arm
.section .text

.globl run_elf

run_elf:
	mov sp, r1
	mov pc, r2
