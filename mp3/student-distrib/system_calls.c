#include "system_calls.h"

//fops_t fops[5];

#define PCB_MAP_SIZE 6

// bit map for available pcb's
// uint8_t pcb_map[PCB_MAP_SIZE];


/*
 * init_fops
 *   DESCRIPTION: Fills the fops array with function pointers.
 *   INPUTS: none			 
 *   OUTPUTS: Places function pointers for all drivers into the fops array.
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: fops is initialized.
 */ 
void init_fops(void)
{

	// STDIN
	fops[PCB_STDIN].open = &terminal_open;
	fops[PCB_STDIN].read = &terminal_read; // true
	fops[PCB_STDIN].write = (int32_t (*)(int32_t, void*, int32_t))&terminal_invalid; // terminal error
	fops[PCB_STDIN].close = &terminal_invalid; // terminal error

	// terminal fops
	// STDOUT
	fops[PCB_STDOUT].open = &terminal_open;
	fops[PCB_STDOUT].read = (int32_t (*)(int32_t, void*, int32_t))&terminal_invalid; // terminal error
	fops[PCB_STDOUT].write = &terminal_write; // true
	fops[PCB_STDOUT].close = &terminal_invalid; // terminal error

	// RTC fops
	fops[RTC_OPS].open = &rtc_open;
	fops[RTC_OPS].read = &rtc_read;
	fops[RTC_OPS].write = (int32_t (*)(int32_t, void*, int32_t))&rtc_write;
	fops[RTC_OPS].close = &rtc_close;

	//dir fops
	fops[FS_DIR_OPS].open = &fs_open;
	fops[FS_DIR_OPS].read = &fs_read_dir;
	fops[FS_DIR_OPS].write = &fs_write;
	fops[FS_DIR_OPS].close = &fs_close;

	//file fops
	fops[FS_FILE_OPS].open = &fs_open;
	fops[FS_FILE_OPS].read = &fs_read_file;
	fops[FS_FILE_OPS].write = &fs_write;
	fops[FS_FILE_OPS].close = &fs_close;
}



/*
	Physical Memory Map
			+-----------------------+
 		0x0 |						|
			|-----------------------+
			|		...				|
			|-----------------------+
			|	kernel code 		|
			|		|				|
			|	   \/ grows down	|
			|-----------------------+
		4MB	|						|		< kernel stacks
			|-----------------------+		<
	8MB-24kB|		PCB2			|		<
			+-----------------------+		<
			|	next stacks			|		<
			|	(ls)				|		<
			+-----------------------+		<
	8MB-16kB|		PCB1			|		<
			+-----------------------+		<
			| testprint stack		|		<
			+-----------------------+		<
	8MB-8kB	|		PCB0			|		<
			+-----------------------+
			|	   ^ grows up		|
		8MB	|	kernel stack		|
			+-----------------------+
8MB to 12MB	|	userspace code		|
			|	(shell)				|
			+-----------------------+
12MB to 16MB|	the next process	|

**** Every process thinks it's at 128MB in the virtual memory map. ****



*/



/*
 * sys_get_PCB_addr
 *   DESCRIPTION: Calculates and returns the appropriate PCB address given an index
 *   INPUTS: index -- the index of the PCB process 
 *   OUTPUTS: none
 *   RETURN VALUE: address of the PCB at 'index'
 *	 SIDE EFFECTS: none
 */ 
int32_t sys_get_PCB_addr(const uint32_t index)
{
	return PCB_BASE_ADDR - ( PCB_OFFSET*(index+1) );
}



/*
 * sys_execute
 *   DESCRIPTION: The execute system call attempts to load and execute a new program, handing off the processor to the new program
 *                until it terminates
 *   INPUTS: command -- a space-separated sequence of words. The ﬁrst word is the ﬁle name of the
 *           program to be executed, and the rest of the command—stripped of leading spaces—should be provided to the new
 *           program on request via the getargs system call
 *   OUTPUTS: Sets up the process control block
 *   RETURN VALUE: The execute call returns -1 if the command cannot be executed,
 *				   for example, if the program does not exist or the ﬁlename speciﬁed is not an executable, 256 if the program dies by an
 *				   exception, or a value in the range 0 to 255 if the program executes a halt system call, in which case the value returned
 *				   is that given by the program’s call to halt
 *	 SIDE EFFECTS: Process control blocks are altered
 */ 
int32_t sys_execute(const uint8_t* command)
{
	int32_t fd;
	uint8_t filename[FILENAME_BUFFER_SIZE];
	uint32_t i = 0;	/* parsing string index */
	uint32_t j = 0;	/* parsing pcb->args index */
	dir_entry_t dentry;
	int32_t retval = 0;
	

	if (active_terminal->top_pcb == NULL)
	{
		strcpy((int8_t *)filename, "shell");
	}
	else
	{
		// check if we have a valid string and first char 
		// check: NULL ptr, NULL char, endline, space
		if( command == NULL || command[i] == '\0' || command[i] == '\n' || command[i] == ' ')
			return -1;

		/* pointer in valid user stack? */
		if( !in_allowed_VM(command) )
			return -1;
	

			/* Parse Filename */

		// get the file name
		while( command[i] != '\0' && command[i] != ' ' && command[i] != '\n' && i < FILENAME_BUFFER_SIZE )
		{
			filename[i] = command[i];
			i++;
		}
		
		filename[i] = '\0'; // end the string with a null
	}
	
			/* Check if executable (ELF) */
	
	 if( -1 == fs_open(filename) )
	 	return -1;

	/* check if file exists */
 	if( -1 == read_dentry_by_name( filename, &dentry) )
 		return -1;  //return fail if read dentry fails

	/* file_desc.file_position is zero */
	uint8_t elf_buffer[ELF_BUF_SIZE];
	if (-1 == read_data( dentry.inode_id, ELF_OFFSET, elf_buffer, ELF_SIZE) )
		return -1;

	elf_buffer[3] = '\0';

	/* Check if executable file */
	if( strncmp( (int8_t *)elf_buffer, (int8_t *)"ELF", ELF_SIZE) != 0 )
		return -1;  //not an executable

		
			/* PCB */	 
	uint8_t new_pid;
	
	/* find available PID */
	for (new_pid = 0; new_pid < MAX_PROC; new_pid++)
	{
		cli();
		if ((pid_mask & (0x1 << new_pid)) == 0x0)
		{
			/* we have found an available pid */
			pid_mask |= (0x1 << new_pid); //set to not available
			sti();
			break;
		}
		sti();
	}
	
	if (new_pid == MAX_PROC)
		return -1; //we are running the maximum number of processes
	
	//printf("New PID: %d\n", new_pid);
	
	/* make sure new PID is visible */
	set_pid_visible(new_pid);
	
	// Set the address of the pcb based on the new pid
	((pcb_t *) sys_get_PCB_addr( new_pid ))->parent_pcb = active_terminal->top_pcb; //set parent pcb first
	active_terminal->top_pcb = (pcb_t *) sys_get_PCB_addr( new_pid );

	active_terminal->top_pcb->pid = new_pid;
	
	
		/* set up stdin */		
	/* keyboard file descriptor struct */
	active_terminal->top_pcb->file[PCB_STDIN].file_ops = (fops_t *)(&fops[PCB_STDIN]); /* keyboard jump table */
	active_terminal->top_pcb->file[PCB_STDIN].inode_index = -1;
	active_terminal->top_pcb->file[PCB_STDIN].file_position = 0;
	active_terminal->top_pcb->file[PCB_STDIN].flags = FILE_OPEN;

		/* set up stdout */
	/* terminal file descriptor struct */
	active_terminal->top_pcb->file[PCB_STDOUT].file_ops = (fops_t *)(&fops[PCB_STDOUT]); /* terminal jump table */
	active_terminal->top_pcb->file[PCB_STDOUT].inode_index = -1;
	active_terminal->top_pcb->file[PCB_STDOUT].file_position = 0;
	active_terminal->top_pcb->file[PCB_STDOUT].flags = FILE_OPEN;

		/* Clear out the rest of the file desc array */
	for(fd=2; fd < FD_SIZE; fd++)
	{
		active_terminal->top_pcb->file[fd].file_ops = NULL;
		active_terminal->top_pcb->file[fd].inode_index = -1;
		active_terminal->top_pcb->file[fd].file_position = 0;
		active_terminal->top_pcb->file[fd].flags = 0;
	}

			/* Parse Arguments */
	if (active_terminal->top_pcb->parent_pcb != NULL)
	{
		// clear argument buffer
		for (j = 0; j < ARG_BUFFER_SIZE; j++)
		{
			active_terminal->top_pcb->args[j] = '\0';
		}

		// if we have some potential args...
		// continue from the i we left off from
		// increment past the char with the NULL/space/endline
		i++;
		j = 0;
		if(command[i] != '\n' || command[i] != '\0') // same as filename[i] == ' '
		{
			while( i < ARG_BUFFER_SIZE && command[i] != '\n' && command[i] != '\0')
			{
				active_terminal->top_pcb->args[j] = command[i];
				i++;
				j++;
			}
			if (i < ARG_BUFFER_SIZE)
				active_terminal->top_pcb->args[j] = '\0';
		}
	}
	
			/* set new page directory */
	setCR3(active_terminal->top_pcb->pid);
	
			/* File loader */
	uint8_t eip_buf[4];
	uint32_t eip = 0;
	uint32_t length;

	length = get_file_size(dentry.inode_id);

	read_data(dentry.inode_id, 24, eip_buf, 4);
	eip = (eip_buf[3] << 24) | (eip_buf[2] << 16) | (eip_buf[1] << 8) | (eip_buf[0] );
	read_data(dentry.inode_id, 0, (uint8_t *)VM_USER_SPACE_PROG, length);
	
	
	
			/* Context Switch
			 * moves from kernel priviledge -> user */

	tss.esp0 = KERNEL_SPACE_START - (PCB_OFFSET * active_terminal->top_pcb->pid);



	//set esp and ebp in the pcb
	asm volatile   ("MOVL %%esp, %0;"				// store esp for restore
					"MOVL %%ebp, %1;"
					:"=m"(active_terminal->top_pcb->parent_esp), "=m"(active_terminal->top_pcb->parent_ebp)	/* outputs: old esp and ebp for restoring */
					:"r"(eip) 			/* input eip */
					);

					
	asm volatile   ("MOVW $0x2B, %%BX;"			//set DS to user data/stack segment
					"MOVW %%BX, %%DS;"
					"PUSHL $0x2B;"				//push user data/stack segment for SS
					"MOVL $0x83FFFF9, %%EBX;"	//push esp (132MB - 4 bytes virtual) as ESP
					// $0x83FFFFC
					"PUSHL %%EBX;"
					"PUSHF;"	
					"PUSHL $0x23;"				//push user code segment as CS
					"PUSHL %1;"					//push eip
					"IRET;"
					"HALT_JUMP:"				// RETURN VALUE EAX IS TAKEN CARE OF IN HALT
					// GDB confirmed this mask is correct
					"ANDL $0xFFFFE000, %%esp;" // mask esp to point to top of stack where PCB is stored		
					"MOVL 4(%%esp), %%ebp;"		// restore ebp and esp
					"MOVL (%%esp), %%esp;"		// don't need to pop args. IRET does.
					"MOVL %%eax, %0;"		// move return value from eax to retval
				//	"leave;"
				//	"iret;"
					: "=r"(retval)					/* output return value */
					:"r"(eip)
					// previous line page faults when "m"(pcb->parent_esp), "m"(pcb->parent_ebp)
					// but triple faults: "r"(old_esp), "r"(old_ebp)
					:"%ebx" 			/* clobbered register */
					);
	
	/* free pid on mask */
	pid_mask &= ~(0x1 << active_terminal->top_pcb->pid);
	
	if (active_terminal->top_pcb->parent_pcb == NULL)
	{
		active_terminal->top_pcb = NULL;
		printf("This is the last shell in the current terminal\n");
		printf("Restarting Shell...\n");
		sys_execute(NULL);
		//printf("Halted\n");
		//cli();
		//asm volatile(".1: hlt; jmp .1;");
	}
	
	active_terminal->top_pcb = active_terminal->top_pcb->parent_pcb;
	
	// change esp0 back to the one it should be at
	tss.esp0 = KERNEL_SPACE_START - (PCB_OFFSET * active_terminal->top_pcb->pid);

	// Set pages back to parent process
	setCR3(active_terminal->top_pcb->pid);

	return retval;
}

/*
 * sys_halt
 *   DESCRIPTION: halts current process, returns to parent process
 *   INPUTS: status--???
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *	 SIDE EFFECTS: none
 */ 
int32_t sys_halt(uint8_t status)
{
	uint32_t i;

	// close all open files in the halting process
	for(i=0; i < FD_SIZE; i++)
	{
		if(active_terminal->top_pcb->file[i].file_ops != NULL)
			active_terminal->top_pcb->file[i].file_ops->close(i);
	}
	
	asm volatile   (
			"XORL %%eax, %%eax;" // clear eax
			"MOVB %0, %%al;"	// move status to eax (return value)
			"JMP HALT_JUMP;"
			:  // no outputs
			:"r"(status) // inputs 
			: "%ax"// clobbers
			);

	//gdb -- x/5i $eip -- disassemble next 5 instr.
	//si step instruction

	return 0;
}



/*
 * sys_open
 *   DESCRIPTION: Performs many sanity checks and then opens a file
 *   INPUTS: filename -- The file to be opened
 *   OUTPUTS: none
 *   RETURN VALUE: fd -- the file descriptor
 *	 SIDE EFFECTS: The file descriptor is modified
 */ 
int32_t sys_open(const uint8_t* filename)
{
	int32_t fd = 2;
	dir_entry_t dentry;

	/* check if filename is valid and within allowed memory bounds */
	if( filename == NULL || !in_allowed_VM(filename) )
		return -1;


	 /* initializes boot block  */
 	// fs_open(filename);

 	// read dentry by name instead of fs_open
	if( -1 == read_dentry_by_name(filename, &dentry) )
		return -1;


	// Find a spot in the unnamed/fd array to put our new file

 	while( active_terminal->top_pcb->file[fd].flags != FILE_CLOSED && fd < 8 )// pcb->file[fd].file_ops != NULL && fd < 8)
 	{
 		fd++;
 	}


 	// continue if we have a valid file descriptor
 /*	if( fd < 8)
 	{
		
		// if it's not a fs descriptor, we want to open it
		// infinite fs_open if we don't check this
		if( dentry.file_type != TYPE_DIR || dentry.file_type != TYPE_FILE )
		{
 			pcb->file[fd].file_ops->open(filename);
 		}
 	}
 
 	else
 */
 	if( fd >= 8)
 	{
 		fd = -1;
 	}
 	else
 	{
 		// initial values to set - inode_index should be -1 to indicate
 		// that our file[fd] is not a dir or file

		active_terminal->top_pcb->file[fd].inode_index = -1;
		active_terminal->top_pcb->file[fd].file_position = 0;
		active_terminal->top_pcb->file[fd].flags = FILE_OPEN;

 		if( dentry.file_type == TYPE_DIR || dentry.file_type == TYPE_FILE)
 		{
 			active_terminal->top_pcb->file[fd].inode_index = dentry.inode_id;
 		}

 		/* assigns the correct file operations pointer to the corresponding file desc */
 		/* positions 0 and 1 are STDIN and STDOUT, respectively */
 		active_terminal->top_pcb->file[fd].file_ops = (fops_t *)&fops[ dentry.file_type + FOPS_OFFSET ];
		
		/* infinite open possible ? Not anymore! */
		//	if( dentry.file_type != TYPE_DIR || dentry.file_type != TYPE_FILE )
		active_terminal->top_pcb->file[fd].file_ops->open(filename);
	}


 	return fd;
}

// pass fd OR pass pointer to file[fd]
/*
 * sys_read
 *   DESCRIPTION: calls read on approriate driver
 *   INPUTS: fd -- the file descriptor
 *			 buf -- char array that is written to
 *			 nbytes -- number of bytes to be read
 *   OUTPUTS: none
 *   RETURN VALUE: the number of bytes read
 *	 SIDE EFFECTS: none
 */ 
int32_t sys_read(int32_t fd, void* buf, int32_t nbytes)
{
	/* check if buf is valid and within allowed memory bounds, and fd is in bounds */
	if( buf == NULL || !in_allowed_VM(buf) || fd < 0 || fd > 8)
		return -1;

	/* check if file is open or closed */
	if( active_terminal->top_pcb->file[fd].flags == FILE_CLOSED)
		return -1;

 	return active_terminal->top_pcb->file[fd].file_ops->read(fd, buf, nbytes);
}
/*
 * sys_write
 *   DESCRIPTION: calls write on approriate driver
 *   INPUTS: fd -- the file descriptor
 *			 buf -- char array to be written
 *			 nbytes -- number of bytes to be written
 *   OUTPUTS: none
 *   RETURN VALUE: the number of bytes written 
 *	 SIDE EFFECTS: none
 */ 
int32_t sys_write(int32_t fd, void* buf, int32_t nbytes)
{
	/* check if buf is valid and within allowed memory bounds, and fd is in bounds*/
	if( buf == NULL || !in_allowed_VM(buf) || fd < 0 || fd > 8)
		return -1;

	/* check if file is open or closed */
	if( active_terminal->top_pcb->file[fd].flags == FILE_CLOSED)
		return -1;

	return active_terminal->top_pcb->file[fd].file_ops->write(fd, buf, nbytes);
}
/*
 * sys_close
 *   DESCRIPTION: calls close on approriate driver
 *   INPUTS: fd -- the file descriptor
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *	 SIDE EFFECTS: none
 */ 
int32_t sys_close(int32_t fd)
{
	/* check if fd is within allowed range */
	if( fd < 2 || fd > 8)
		return -1;

	/* check if file is open or closed */
	if( active_terminal->top_pcb->file[fd].flags == FILE_CLOSED)
		return -1;

	int32_t retval = active_terminal->top_pcb->file[fd].file_ops->close(fd);


	active_terminal->top_pcb->file[fd].file_ops = NULL;
	active_terminal->top_pcb->file[fd].inode_index = -1;
	active_terminal->top_pcb->file[fd].file_position = 0;
	active_terminal->top_pcb->file[fd].flags = FILE_CLOSED;
	
	return retval;
}


/*
 * in_allowed_VM
 *   DESCRIPTION: tells the user whether or not an address is in the allowed address space
 * 				  for the userspace
 *   INPUTS: string -- address in question
 *   OUTPUTS: none
 *   RETURN VALUE: 1 on success, 0 on failure
 *	 SIDE EFFECTS: none
 */ 
int32_t in_allowed_VM(const uint8_t* string)
{
	if( (int32_t)string < VM_USER_SPACE_START || (int32_t)string >= VM_USER_SPACE_END)
	{
		return 0;
	}
	else
	{
		return 1;
	}

}


/*
 * sys_getargs
 *   DESCRIPTION: Get arguments from pcb that were stored during execute parsing.
 *   INPUTS: buf    -- char array to fill
 *			 nbytes -- number of bytes to copy to buf
 *   OUTPUTS: none
 *   RETURN VALUE: Returns 0 on success, or -1 on fail
 *	 SIDE EFFECTS: buf is modifed and filled with arguments parsed from execute
 */ 
int32_t sys_getargs(uint8_t* buf, int32_t nbytes)
{
	uint32_t i, length=0;

	if(buf == NULL || !in_allowed_VM(buf) )
		return -1;

	for (i = 0; i < 128; i++) {
		if (active_terminal->top_pcb->args[i] == '\n' || active_terminal->top_pcb->args[i] == '\0') {
			length = i + 1;
			break;
		}
	}
	if (length > nbytes) return -1;
	
	for (i = 0; i < length; i++) {
		buf[i] = active_terminal->top_pcb->args[i];
	}
	
	return 0;
}
/*
 * sys_vidmap
 *   DESCRIPTION: Maps text-mode video memory into user space
 *   INPUTS: screen_start -- the location in vid mem the caller wants to map to
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on fail
 *	 SIDE EFFECTS: changes screen_start
 */ 
int32_t sys_vidmap(uint8_t** screen_start)
{
	uint32_t video_addr;
	video_addr = VID_MEM_USER;
	
	/* check pointer for validity */
	if (!in_allowed_VM((uint8_t *)screen_start))
		return -1;

	//test_temp = memcpy((void *)(screen_start), (void *)(&video_addr), POINTER_SIZE);
	*screen_start = (uint8_t *)video_addr;
	return 0;
}
/*
 * sys_set_handler
 *   DESCRIPTION: Unimplemented extra credit
 *   INPUTS: signum-- ???
 *           handler_address-- ???
 *   OUTPUTS: none
 *   RETURN VALUE: -1 on fail
 *	 SIDE EFFECTS: none
 */ 
int32_t sys_set_handler(int32_t signum, void* handler_address)
{
	return -1;
}
/*
 * sys_sigreturn
 *   DESCRIPTION: Unimplemented extra credit
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1 on fail
 *	 SIDE EFFECTS: none
 */ 
int32_t sys_sigreturn(void)
{
	return -1;
}
