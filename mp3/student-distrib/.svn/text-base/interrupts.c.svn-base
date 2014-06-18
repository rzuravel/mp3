#include "interrupts.h"
#include "initPaging.h"


/*		Keyboard Globals
 * ============================
 */
static uint8_t curr_key;
static uint8_t shift_down = 0;
static uint8_t ctrl_down = 0;
static uint8_t alt_down = 0;
static uint8_t caps_on = 0;
/* initialize the scancode to keycode array 
	[2 * SHIFT_OFFSET] to create enough room */
uint8_t keycodes[2 * SHIFT_OFFSET] = { 0,0,'1','2','3','4','5','6','7','8','9','0','-','=',
									  '\b','\t','q','w','e','r','t','y','u','i','o','p','[',
									  ']','\n',0,'a','s','d','f','g','h','j','k','l',';','\'',
									  '`',0,'\\','z','x','c','v','b','n','m',',','.','/',0,
									  '*',0,' ',0,0,'!','@','#','$','%','^','&','*','(',')','_',
									  '+','\b','\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
									  0,'A','S','D','F','G','H','J','K','L',':','"','~',0,'|',
									  'Z','X','C','V','B','N','M','<','>','?',0,0,0,' ' };



extern int32_t t_buf_pos;


/*
 * keyboard_init
 *   DESCRIPTION: Initialize the keyboard for use (enables interrupts).
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Sends enable command to keyboard and clears curr_key.
 */ 
void keyboard_init()
{
	/* Sends enable command to keyboard controller */
	outb(KEYBOARD_CTL_WRITE_PORT, KEYBOARD_CMD_ENABLE);
	
	curr_key = 0x00;
	
	enable_irq(KEYBOARD_IRQ); /* Enable keyboard interrupt */
	
	
}


/*
 * keyboard_handler
 *   DESCRIPTION: Gets character that was pressed on keyboard.
 *				---- For MP3.1: prints char to screen -----
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Grabs character from keyboard buffer. Prints to screen.
 */ 
void keyboard_handler()
{
	uint32_t scancode;
	
	/* read scancode */
	scancode = inb(KEYBOARD_READ_INPUT_BUFFER_PORT);
	
	/* convert scancode to char */
	curr_key = get_scancode(scancode);
	
	//cli();
	/* output to terminal if not null */
	if( curr_key != 0 && !(curr_key == '\b' && visible_terminal->t_buf_pos == 0)) {
		get_screen_pos(&(active_terminal->screen_x), &(active_terminal->screen_y));
		set_screen_pos(visible_terminal->screen_x, visible_terminal->screen_y);
		
		putc(curr_key, 1);
		terminal_new_char(curr_key);	//write this char on the terminal buffer
		move_cursor();
		
		get_screen_pos(&(visible_terminal->screen_x), &(visible_terminal->screen_y));
		set_screen_pos(active_terminal->screen_x, active_terminal->screen_y);
	}
	//sti();

	send_eoi(KEYBOARD_IRQ); /* We're done w/ handling keyboard */
}
/*
 * get_scancode
 *   DESCRIPTION: Helper function that converts keyboard scancode into a character.
 *   INPUTS: letter -- value to convert
 *   OUTPUTS: none
 *   RETURN VALUE: converted character that corresponds to keyboard button/character
 *				   int 0 if unrecognized character
 *	 SIDE EFFECTS: none
 */ 
uint8_t get_scancode(uint8_t letter)
{
	switch(letter)
	{
		/* modifier keys */
		case 0x2A:				//left shift
			shift_down = 1;
			return 0;
		case 0xAA:
			shift_down = 0;
			return 0;
		case 0x36:				//right shift
			shift_down = 1;
			return 0;
		case 0xB6:
			shift_down = 0;
			return 0;
		case 0x38:				//alt
			alt_down = 1;
			return 0;
		case 0xB8:
			alt_down = 0;
			return 0;
		case 0x1D:				//control (left and right are the same)
			ctrl_down = 1;
			return 0;
		case 0x9D:
			ctrl_down = 0;
			return 0;
		case 0x3A:				//caps lock
			caps_on ^= 1;
			return 0;
		/* function keys */
		case 0x3B:				//F1
			if (alt_down && valid_login) {	// valid_login used to authenticate: 1 = passed
				send_eoi(KEYBOARD_IRQ);
				terminal_switch(0);
			}
			return 0;
		case 0x3C:				//F2
			if (alt_down && valid_login) { // valid_login used to authenticate: 1 = passed
				send_eoi(KEYBOARD_IRQ);
				terminal_switch(1);
			}
			return 0;
		case 0x3D:				//F3
			if (alt_down && valid_login) { // valid_login used to authenticate: 1 = passed
				send_eoi(KEYBOARD_IRQ);
				terminal_switch(2);
			}
			return 0;
		/* character keys */
		case 0x26:
			if (ctrl_down && valid_login) {
				get_screen_pos(&(active_terminal->screen_x), &(active_terminal->screen_y));
				set_screen_pos(visible_terminal->screen_x, visible_terminal->screen_y);
				
				clear(1);
				move_cursor();
				
				get_screen_pos(&(visible_terminal->screen_x), &(visible_terminal->screen_y));
				set_screen_pos(active_terminal->screen_x, active_terminal->screen_y);
				return 0;
			}
			if (shift_down ^ caps_on) return 'L';
			return 'l';
		default:
			if ( !ctrl_down && letter > 0x00 && letter < 2 * SHIFT_OFFSET )
			{
				uint8_t letter_cap = 0;
				if(caps_on && ((letter > TAB && letter < LEFT_BRACE) || (letter > CTRL_LEFT && letter < SEMICOLON) || (letter > BACKSLASH && letter < COMMA))) letter_cap = 1;
				return keycodes[letter + ((shift_down ^ letter_cap) * SHIFT_OFFSET)];
			}
	}

	/* Fallback if input is non-recognized character (for mp3.1, at least) */
	return 0;
}
