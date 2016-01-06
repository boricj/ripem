.arm
.section .startup, "ax"

.extern _bss_start
.extern _bss_end

_entry:
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

	ldr r1, =main
	mov pc, r1
