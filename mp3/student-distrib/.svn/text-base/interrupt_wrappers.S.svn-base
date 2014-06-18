# interrupt_wrappers.S 
# These are assembly wrappers for interrupts: pit, keyboard and rtc.
# These are necessary so that we can save the current registers and transition to C.

.text

.globl wrap_pit_interrupt
.globl wrap_keyboard_interrupt
.globl wrap_rtc_interrupt

wrap_pit_interrupt:
	
	pushal
	call pit_handler
	popal
	iret

wrap_keyboard_interrupt:
	pushal
	call keyboard_handler
	popal
	iret


wrap_rtc_interrupt:
	pushal
	call rtc_handler
	popal
	iret
