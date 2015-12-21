.arm
.pushsection .text.startup

_entry:
	bl main
_hang:
	b _hang

.popsection
