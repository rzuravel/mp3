#ifndef _SYSTEM_CALLS_H
#define _SYSTEM_CALLS_H

#include "x86_desc.h"
#include "filesystem.h"
#include "initPaging.h"
#include "terminal_driver.h"
#include "rtc.h"

#define FD_SIZE 8


/* Index for File Operations Table Pointers */
#define PCB_STDIN 0
#define PCB_STDOUT 1

#define KEY_OPS 0
#define TERM_OPS 1
#define RTC_OPS 2
#define FS_DIR_OPS 3
#define FS_FILE_OPS 4

#define FOPS_OFFSET 2
#define FOPS_SIZE 5

/* Used for file system dentry file types */
#define TYPE_RTC 0
#define TYPE_DIR 1
#define TYPE_FILE 2

#define FILE_OPEN 1
#define FILE_CLOSED 0

#define MAX_PROC 6

/* Used for setting PCB */
#define PCB_BASE_ADDR (8*(1<<20)) // - 8192) /*  (Eric told us this) */
#define PCB_OFFSET 8192
#define KERNEL_SPACE_START 8*(1<<20)\

/* 4MB page user is allowed to use in VM */
#define VM_USER_SPACE_START 128*(1<<20) /* 128 MB */
#define VM_USER_SPACE_END 132*(1<<20) /* 132 MB */
#define VM_USER_SPACE_PROG 0x08048000

/* Legacy define */
#define VM_KERNEL_START_ADDR 4*(1<<20)

/* vidmem define */
#define POINTER_SIZE 4


#define FILENAME_BUFFER_SIZE 32
#define ARG_BUFFER_SIZE 128




/* Verifying file is an executable */
#define ELF_OFFSET 1
#define ELF_SIZE 3
#define ELF_BUF_SIZE 32

/* forces compiler to look for inode struct declaration */
typedef struct inode inode_t;


typedef struct file_operations
{
	// commas or semicolons at end?
	int32_t (*open)(const uint8_t* filename);
	int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
	int32_t (*write)(int32_t fd, void* buf, int32_t nbytes);
	int32_t (*close)(int32_t fd);
} fops_t;


typedef struct file_desc 
{
	// file operations table pointer - points to jump table
	fops_t * file_ops;
	uint32_t inode_index;
	uint32_t file_position;
	uint32_t flags;		// use to see if file is open
} file_desc_t;

/* one for each process, stores vital process info */
typedef struct process_control_block
{	
	// Change the order AND OUR EXECUTE/HALT (and you, too) DIES!!! 
	uint32_t parent_esp;						//parent_esp and ebp are used to return to the parent process
	uint32_t parent_ebp;
	struct process_control_block * parent_pcb;	//points back to the parent process
	file_desc_t file[FD_SIZE];					//file descriptors for each possible open file
	uint32_t pid;								//process id number
	
	uint8_t args[ARG_BUFFER_SIZE];				//string for arguments from call from shell
} pcb_t;

uint8_t pid_mask;

//pcb_t * pcb;
fops_t fops[FOPS_SIZE];
// extern file_desc_t file_desc;

extern void init_fops(void);
extern int32_t sys_get_PCB_addr(const uint32_t index);
extern int32_t in_allowed_VM(const uint8_t* string);
extern int32_t is_valid_exe_string(const uint8_t* string);

extern int32_t sys_open(const uint8_t* filename);
extern int32_t sys_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t sys_write(int32_t fd, void* buf, int32_t nbytes);
extern int32_t sys_close(int32_t fd);

extern int32_t sys_execute(const uint8_t* command);

extern int32_t sys_halt(uint8_t status);
extern int32_t sys_getargs(uint8_t* buf, int32_t nbytes);
extern int32_t sys_vidmap(uint8_t** screen_start);
extern int32_t sys_set_handler(int32_t signum, void* handler_address);
extern int32_t sys_sigreturn(void);


#endif
