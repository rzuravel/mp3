/* pit.c - File for the PIT driver
*/

#include "pit.h"
#include "terminal_driver.h"

static uint32_t terminal_index;

/*
 * pit_init
 *   DESCRIPTION: Initialize the PIT to send interrupts to the PIC at 18 Hz
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: initialize and activate the PIT
 */ 
void pit_init()
{
	cli();
	outb(MODE_VAL,PIT_MODE_REG);	
	outb(SET_HZ,PIT_MODE_REG);
	outb(HZ_SET & 0xFF, PIT_CHANNEL_0);
	outb(HZ_SET & 0xFF00, PIT_CHANNEL_0);
	
	terminal_index = 0;

	enable_irq(PIT_IRQ); /* Enable pit interrupt and slave PIC */
	sti();
}

/*
 * pit_handler
 *   DESCRIPTION: Called by wrapper when IRQ 0 occurs
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Switches active task (on each call) to give other processes a slice of running time
 */ 
void pit_handler()
{
	cli();

	/* find next task to run */
	terminal_index = (terminal_index + 1) % NUM_TERMINALS;
	while (terminals[terminal_index].top_pcb == NULL && &terminals[terminal_index] != visible_terminal)
		terminal_index = (terminal_index + 1) % NUM_TERMINALS;

	/* if it's already the active terminal, just exit */
	if (&terminals[terminal_index] != active_terminal)
	{
	
		//get and save esp and ebp in the terminal_instance so we can return back
		asm volatile   ("MOVL %%esp, %0;"				// store esp for restore
						"MOVL %%ebp, %1;"
						:"=m"(active_terminal->return_esp), "=m"(active_terminal->return_ebp)
						);
						
		/* save screen position */
		get_screen_pos(&(active_terminal->screen_x), &(active_terminal->screen_y));
			
		/* set the new terminal as active */
		active_terminal = &terminals[terminal_index];
		
		/* restore screen position and set cursor */
		set_screen_pos(active_terminal->screen_x, active_terminal->screen_y);
		if (active_terminal == visible_terminal)
			move_cursor();
		
		/* this terminal has not been started yet, start shell */
		if (terminals[terminal_index].top_pcb == NULL)
		{
			send_eoi(PIT_IRQ);
			sti();
			if (sys_execute(NULL) == -1)
			{
				clear(1);
				printf("Too many processes running, please close a process to use this terminal");
				asm volatile(".1: hlt; jmp .1;");
			}
			return;
		}
		
		//set esp and ebp to return back
		asm volatile   ("MOVL %0, %%esp;"				// store esp for restore
						"MOVL %1, %%ebp;"
						:	/* no outputs */
						:"m"(active_terminal->return_esp), "m"(active_terminal->return_ebp)
						);
						
		/* set TSS */
		tss.esp0 = KERNEL_SPACE_START - (PCB_OFFSET * active_terminal->top_pcb->pid);	
		
		/* set CR3 */
		setCR3(active_terminal->top_pcb->pid);
	}
	
	// uncomment this for FUN MODE
	// attrib = ((attrib & 0xF0) + 0x10 ) | ((attrib & 0x0F)+ 0x01);

	send_eoi(PIT_IRQ);
	sti();
}
