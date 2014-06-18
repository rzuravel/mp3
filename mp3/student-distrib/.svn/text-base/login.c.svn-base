#include "login.h"

#define SHORT_BUF_SIZE 16
#define BIG_BUF_SIZE 128

#define INVALID 0
#define VALID 1

/*
 * authenticate
 *   DESCRIPTION: Authenticates user for OS login
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: allows the user to continue to the OS shell
 */ 
void authenticate()
{
	attrib = 0x13; // login screen colors
	clear(1);
	valid_login = INVALID; // disables the use of multiple terminals and Ctrl+L (clear) to bypass authenticate

	// if by chance the buffer protection fails (it should not read more than 127 bytes),
	// it will be a fun challenge to work through these other buffers
	
	uint8_t buf[BIG_BUF_SIZE] = "";
	uint8_t buf_tainted1[SHORT_BUF_SIZE] = NOT_TAINTED;
	uint8_t sneaky_buf[BIG_BUF_SIZE] = LOGIN_PASS;
	uint8_t buf_tainted2[SHORT_BUF_SIZE] = NOT_TAINTED;
	uint32_t read = LOGIN_PASS_SIZE;
	printf("Please login\n");

	move_cursor();
	while(  strncmp( (int8_t*)buf, (int8_t*)sneaky_buf, read) != 0 &&
			strncmp( (int8_t*)buf_tainted1, (int8_t*)NOT_TAINTED, SHORT_BUF_SIZE) == 0 &&
			strncmp( (int8_t*)buf_tainted2, (int8_t*)NOT_TAINTED, SHORT_BUF_SIZE) == 0)
	{
			printf("Password: ");
			move_cursor();
			read = terminal_read(0,buf,BIG_BUF_SIZE-1);

			if( read < LOGIN_PASS_SIZE )
				read = LOGIN_PASS_SIZE;
			else
			{
				read--;
				buf[read] = '\0';
			}
				
	}


	valid_login = VALID; // restores multiple terminals and Ctrl+L (clear)
	attrib = 0x02;	// normal OS colors
	clear(1);
	printf("Welcome to Illinix\n");

}
