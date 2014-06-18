/* terminal_driver.h - header file for the terminal driver
 */

#ifndef _TERMINAL_DRIVER_H
#define _TERMINAL_DRIVER_H

#include "types.h"
#include "system_calls.h"


#define NUM_TERMINALS 3

#define TERMINAL_BUFFER_SIZE 	128
#define VIDEO_MEMORY_SIZE		4000

typedef struct terminal_instance
{
	uint32_t index;
	struct process_control_block * top_pcb;
	int32_t * return_esp;
	int32_t * return_ebp;
	/* buffer vars */
	uint8_t t_buf[TERMINAL_BUFFER_SIZE];
	int32_t t_buf_pos;
	uint8_t read_wait;
	/* video memory */
	int32_t screen_x;
	int32_t screen_y;
	//uint8_t vid_mem[VIDEO_MEMORY_SIZE];
} terminal_instance_t;

/* terminal that is getting cpu time */
terminal_instance_t *active_terminal;
/* terminal that is currently being displayed */
terminal_instance_t *visible_terminal; 
/* array for all the terminals */
terminal_instance_t terminals[NUM_TERMINALS];

/* Externally-visible functions */

/* "open" the terminal */
int32_t terminal_open(const uint8_t* filename);

/* read from the terminal */
int32_t terminal_read(int32_t fd, void *buf, int32_t nbytes);

/* Write text out to the terminal */
int32_t terminal_write(int32_t fd, void *buf, int32_t nbytes);

/* "close" the terminal */
int32_t terminal_close(int32_t fd);

/* error function for stdin and stdout, returns -1 */
int32_t terminal_invalid(int32_t fd);

/* edit the buffer from a new keypress, called from the keyboard interrupt handler */
void terminal_new_char(int8_t c);

/* initializes terminals */
void terminal_initialize_terminals(void);

/* switches terminals */
void terminal_switch(uint32_t new_terminal_num);

#endif /* _TERMINAL_DRIVER_H */
