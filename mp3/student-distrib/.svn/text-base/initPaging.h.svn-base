/* initPaging.h - Enables paging for the OS
 */

#ifndef _INITPAGING_H
#define _INITPAGING_H

#include "types.h"
#include "lib.h"



/* constants involved with paging */
#define PAGES_PER_TABLE 		1024
#define TABLES_PER_DIR 			1024
#define	PAGE_SIZE				4096		//4KB per page
#define KERNEL_START_ADDR		0x400000	//kernel starts at 4MB
#define USER_START_ADDR			0x800000	//user space starts at 8MB
#define USER_SPACE_LENGTH		0x400000	//user space each 4MB
#define MAX_PROCESSES			6			//only support 6 processes

/* PDE bit constants */
#define I86_PDE_PRESENT			1			//00000000000000000000000000000001
#define I86_PDE_WRITABLE		2			//00000000000000000000000000000010
#define I86_PDE_USER			4			//00000000000000000000000000000100
#define I86_PDE_PWT				8			//00000000000000000000000000001000
#define I86_PDE_PCD				0x10		//00000000000000000000000000010000
#define I86_PDE_ACCESSED		0x20		//00000000000000000000000000100000
#define I86_PDE_DIRTY			0x40		//00000000000000000000000001000000
#define I86_PDE_4MB				0x80		//00000000000000000000000010000000
#define I86_PDE_CPU_GLOBAL		0x100		//00000000000000000000000100000000
#define I86_PDE_LV4_GLOBAL		0x200		//00000000000000000000001000000000
#define I86_PDE_BASE			0xFFFFF000 	//11111111111111111111000000000000

/* PTE bit constants */
#define I86_PTE_PRESENT			1			//00000000000000000000000000000001
#define I86_PTE_WRITABLE		2			//00000000000000000000000000000010
#define I86_PTE_USER			4			//00000000000000000000000000000100
#define I86_PTE_WRITETHOUGH		8			//00000000000000000000000000001000
#define I86_PTE_NOT_CACHEABLE	0x10		//00000000000000000000000000010000
#define I86_PTE_ACCESSED		0x20		//00000000000000000000000000100000
#define I86_PTE_DIRTY			0x40		//00000000000000000000000001000000
#define I86_PTE_PAT				0x80		//00000000000000000000000010000000
#define I86_PTE_CPU_GLOBAL		0x100		//00000000000000000000000100000000
#define I86_PTE_LV4_GLOBAL		0x200		//00000000000000000000001000000000
#define I86_PTE_BASE			0xFFFFF000 	//11111111111111111111000000000000

/* video memory pages */
#define VID_NUM_PAGES 	2
#define VID_MEM_USER 	0x4000000 			//64 MB
#define BACK_BUFFER		0x3000000			//48 MB
#define STATIC_VIDEO	0x4001000			//64 MB + 4KB

/* page directory entry indecies */
#define PHYS_VID_PDE 	0
#define KERNEL_PDE 		1
#define USER_VID_PDE 	16
#define USER_PDE 		32


/* A page directory */
typedef struct page_dir {
	uint32_t pde[TABLES_PER_DIR];
} page_dir;

/* A page table */
typedef struct page_table {
	uint32_t pte[PAGES_PER_TABLE];
} page_table;

/* the array of page directories (one per process) */
page_dir pdir[MAX_PROCESSES]  __attribute__((aligned (4096)));;
/* the page table for the first 4MB (one_per_process) */
page_table pt_phys_vid[MAX_PROCESSES] __attribute__((aligned (4096)));;
/* the page table for the user video memory (one_per_process) */
page_table pt_virt_vid[MAX_PROCESSES] __attribute__((aligned (4096)));;

/* Externally-visible functions */

/* Initialize Paging */
void initPaging(void);
/* Initialize Paging */
void setCR3(int PID);
/* map video memory pages to a back buffer */
void set_pid_invisible(uint32_t PID, uint32_t terminal_index);
/* map video memory pages back to physical memory */
void set_pid_visible(uint32_t PID);

#endif /* _INITPAGING_H */
