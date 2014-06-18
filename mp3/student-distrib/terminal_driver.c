/* terminal_driver.c - Functionto create char buffer for shell
 */

#include "terminal_driver.h"
#include "lib.h"
#include "initPaging.h"


/* "open" the terminal
 * inputs: none
 * outputs: returns 0
 * side effects: initializes constants
 */
int32_t terminal_open(const uint8_t* filename)
{
	return 0;
}

/* read from the terminal
 * inputs: buf -- buffer to write the chars in to
 *         nbytes -- number of bytes to read
 * outputs: none
 * side effects: waits indefinitely for line to be terminated with '\n'
 */
int32_t terminal_read(int32_t fd, void *buf, int32_t nbytes)
{
	int32_t i = 0;
	uint32_t j=0;

	for(j=0; j < TERMINAL_BUFFER_SIZE; j++)
	{
		((int8_t*)buf)[j] = '\0';
	}


	active_terminal->read_wait = 1;
	while (active_terminal->read_wait);
	
	while(1) {
		((uint8_t*)buf)[i] = active_terminal->t_buf[i];
		if (active_terminal->t_buf[i] == '\n')
		{
			return i + 1;
		}
		i++;
		if (i == nbytes)
		{
			return i;
		}
	}
}

/* Write text out to the terminal
 * inputs: buf -- the buffer containing the characters to be written
 * 	       nbytes -- the number of characters to write to the screen	
 * outputs: none
 * side effects: writes characters to the screen
 */
int32_t terminal_write(int32_t fd, void *buf, int32_t nbytes)
{
	int32_t i = 0;
	
	cli();
	for (i = 0; i < nbytes; i++) {
		putc(((uint8_t*)buf)[i], 0);
	}
	
	if (active_terminal == visible_terminal)
		move_cursor();
	
	sti();
	
	return i;
}

/* "close" the terminal
 * inputs: none
 * outputs: returns 0
 * side effects: none
 */
int32_t terminal_close(int32_t fd)
{
	return 0;
}

/* invalid command from stdin or stdout
 * inputs: none
 * outputs: returns -1
 * side effects: none
 */
int32_t terminal_invalid(int32_t fd)
{
	return -1;
}

/* edit the buffer from a new keypress, called from the keyboard interrupt handler
 * inputs: c -- the char for the key that was pressed
 * outputs: none
 * side effects: edits the buffer
 *				 may allow the read function to continue
 */
void terminal_new_char(int8_t c)
{
	if(c == '\b') {
		visible_terminal->t_buf_pos--;
		if (visible_terminal->t_buf_pos < 0)
			visible_terminal->t_buf_pos = 0;
	}else if(c == '\n') {
		if (visible_terminal->read_wait) {
			visible_terminal->t_buf[visible_terminal->t_buf_pos] = c;
			visible_terminal->read_wait = 0;
		} 
		visible_terminal->t_buf_pos = 0;
	}else {
		visible_terminal->t_buf[visible_terminal->t_buf_pos] = c;
		visible_terminal->t_buf_pos++;
		if (visible_terminal->t_buf_pos >= TERMINAL_BUFFER_SIZE)
			visible_terminal->t_buf_pos = TERMINAL_BUFFER_SIZE - 1;
	}
}

/* sets up all the terminal_instance structs to be used
 * inputs: none
 * outputs: none
 * side effects: terminal_structs are initialized
 */
void terminal_initialize_terminals()
{
	uint32_t i; //, j;
	
	active_terminal = &terminals[0];
	visible_terminal = &terminals[0];

	for (i = 0; i < NUM_TERMINALS; i++)
	{
		terminals[i].index = i;
		terminals[i].top_pcb = NULL;
		terminals[i].return_esp = NULL;
		terminals[i].return_ebp = NULL;
		/* buffer vars */
		terminals[i].t_buf_pos = 0;
		terminals[i].read_wait = 0;
	}
}

/* switch visible terminals
 * inputs: new_terminal_num -- the index of the new terminal to switch to
 * outputs: none
 * side effects: If the new visible terminal has not already been used, 
 *  			 it will start shell on it (taking up another pid).
 * 				 Also remaps paging.
 */
void terminal_switch(uint32_t new_terminal_num)
{
	uint32_t j;
	uint8_t* video_mem = (uint8_t *)VIDEO;
	uint8_t* static_video_mem = (uint8_t *)STATIC_VIDEO;
	struct process_control_block * cur_pcb;


	cli();


	/* check new_terminal_num is valid */
	if (new_terminal_num < 0 || new_terminal_num >= NUM_TERMINALS)
	{
		sti();
		return;
	}
		
	/* if it's already the active terminal, just exit */
	if (&terminals[new_terminal_num] == visible_terminal)
	{
		sti();
		return;
	}

//	cli();
			/* SAVE OLD TERMINAL */
	
	if (visible_terminal->top_pcb != NULL)
	{
		/* map old terminal to its back buffer for all processes (starting with bottom child) */
		cur_pcb = visible_terminal->top_pcb;
		while (cur_pcb != NULL) {
			set_pid_invisible(cur_pcb->pid, visible_terminal->index);
			cur_pcb = cur_pcb->parent_pcb;
		}
		
		/* flush TLB */
		setCR3(visible_terminal->top_pcb->pid);
		
		/* copy old contents (in physical memory) into back buffer */
		for(j = 0; j < NUM_ROWS*NUM_COLS; j++) {
			*(uint8_t *)(video_mem + (j << 1))     = *(uint8_t *)(static_video_mem + (j << 1));
			*(uint8_t *)(video_mem + (j << 1) + 1) = *(uint8_t *)(static_video_mem + (j << 1) + 1);
		}
	}
			
			/* SET UP NEW TERMINAL */
			
	/* set the new terminal as active */
	visible_terminal = &terminals[new_terminal_num];
	
	/* if nothing is running on this terminal, prepare to run shell */
	if (visible_terminal->top_pcb == NULL)
	{
		/* can't mess up active terminal x and y */
		get_screen_pos(&(active_terminal->screen_x), &(active_terminal->screen_y));
		clear(1);
		get_screen_pos(&(visible_terminal->screen_x), &(visible_terminal->screen_y));
		set_screen_pos(active_terminal->screen_x, active_terminal->screen_y);
	
		sti();
		return;
	}
	
	
	/* set CR3 to top process on this terminal */
	setCR3(visible_terminal->top_pcb->pid);
	
	
	/* copy contents from back buffer into physical video memory */
	for(j = 0; j < NUM_ROWS*NUM_COLS; j++) {
		*(uint8_t *)(static_video_mem + (j << 1))     = *(uint8_t *)(video_mem + (j << 1));
		*(uint8_t *)(static_video_mem + (j << 1) + 1) = *(uint8_t *)(video_mem + (j << 1) + 1);
	}
	
	/* remap old terminal to physical video memory for all processes (starting with bottom child) */
	cur_pcb = visible_terminal->top_pcb;
	while (cur_pcb != NULL) {
		set_pid_visible(cur_pcb->pid);
		cur_pcb = cur_pcb->parent_pcb;
	}
	
	
	/* restore paging to active terminal */
	/* if executing shell on a newly active terminal
	   fails because of too many processes, top_pcb
	   will still be NULL, in which case we do NOT want
	   to change CR3 to it */
	if (active_terminal->top_pcb != NULL)
		setCR3(active_terminal->top_pcb->pid);
	

	sti();
}
