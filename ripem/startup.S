.arm
.pushsection .text.startup

_vec_reset:
	b _entry_vec_reset
_vec_undef:
	b _entry_vec_undef
_vec_swi:
	b _entry_vec_swi
_vec_pabort:
	b _entry_vec_pabort
_vec_dabort:
	b _entry_vec_dabort
_vec_reserved:
	b _entry_vec_reserved
_vec_irq:
	b _entry_vec_irq
_vec_fiq:
	b _entry_vec_fiq

_entry_vec_reset:
	ldr r15, =vec_reset
_entry_vec_undef:
	ldr r15, =vec_undef
_entry_vec_swi:
	ldr r15, =vec_swi
_entry_vec_pabort:
	ldr r15, =vec_pabort
_entry_vec_dabort:
	ldr r15, =vec_dabort
_entry_vec_reserved:
	ldr r15, =vec_reserved
_entry_vec_irq:
	ldr r15, =vec_irq
_entry_vec_fiq:
	ldr r15, =vec_fiq

vec_reset:	.word hang
vec_undef:	.word hang
vec_swi:	.word hang
vec_pabort:	.word hang
vec_dabort:	.word hang
vec_reserved:	.word hang
vec_irq:	.word hang
vec_fiq:	.word hang

hang:
	b hang

.popsection
