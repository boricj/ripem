.arm
.section .startup, "ax"

.extern _rel_start
.extern _rel_end
.extern _bss_start
.extern _bss_end

_entry:
#
# Make BXCBOOT0.BIN read-only. Just in case.
#
	ldr r1,=0x4E000000
# Make sure at least the first 128 sectors (=256 KiB) are protected.
	ldr r2, [r1, #0x20]
	cmp r2, #0x80
	movlo r2, #0x80
	str r2, [r1, #0x20]
# Turn on software-proof write protection.
	ldr r2, [r1, #0x4]
	orr r2, #0x20000
	str r2, [r1, #0x4]

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
