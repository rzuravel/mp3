/* kernel.c - the C part of the kernel
 * vim:ts=4 noexpandtab
 */


#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "initPaging.h"
#include "debug.h"

#include "exception.h"
#include "interrupts.h"
#include "system_calls.h"
#include "filesystem.h"
#include "rtc.h"
#include "pit.h"
#include "login.h"

/* Macros. */
/* Check if the bit BIT in FLAGS is set. */

#define CHECK_FLAG(flags,bit)  ((flags) & (1 << (bit))) // 0x00

/* size of file system read buffer */
#define BUF_SIZE 32
#define SHELL_PID 0


/*
 *
 *	Exception Handlers
 *	Assembly Wrappers around C functions
 *
*/

extern uint32_t wrap_divide_by_zero_exception;

extern uint32_t wrap_non_maskable_int_exception;

extern uint32_t wrap_breakpoint_exception;

extern uint32_t wrap_overflow_exception;

extern uint32_t wrap_bounds_exceeded_exception;

extern uint32_t wrap_invalid_opcode_exception;

extern uint32_t wrap_device_not_available_exception;

extern uint32_t wrap_double_fault_exception;

extern uint32_t wrap_co_cpu_segment_overrun_exception;

extern uint32_t wrap_invalid_TSS_exception;

extern uint32_t wrap_segment_not_present_exception;

extern uint32_t wrap_stack_segment_fault_exception;

extern uint32_t wrap_general_protection_exception;

extern uint32_t wrap_page_fault_exception;

extern uint32_t wrap_x87_FPU_error_exception;

extern uint32_t wrap_alignment_check_exception;

extern uint32_t wrap_machine_check_exception;

extern uint32_t wrap_SIMD_FPU_exception;

/*
 *	System Assembly Handler
 *
*/
extern uint32_t sys_call_handler;

/*
 *
 *	Interrupt Handlers
 *	Keyboard and RTC Assembly Wrappers
 *
*/
extern uint32_t wrap_keyboard_interrupt;
extern uint32_t wrap_rtc_interrupt;
extern uint32_t wrap_pit_interrupt;





/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void
entry (unsigned long magic, unsigned long addr)
{
	multiboot_info_t *mbi;
	//uint16_t i; /* index used for disabling all interrupts */

	/* Clear the screen. */
	//clear();
	
	/* Enable Paging */
	initPaging();

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf ("Invalid magic number: 0x%#x\n", (unsigned) magic);
		return;
	}

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;

	/* Print out the flags. */
	printf ("flags = 0x%#x\n", (unsigned) mbi->flags);

	/* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0))
		printf ("mem_lower = %uKB, mem_upper = %uKB\n",
				(unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

	/* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		printf ("boot_device = 0x%#x\n", (unsigned) mbi->boot_device);

	/* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
		printf ("cmdline = %s\n", (char *) mbi->cmdline);

	if (CHECK_FLAG (mbi->flags, 3))  {
		int mod_count = 0;
		int i;
		module_t* mod = (module_t*)mbi->mods_addr;

		/* This is very necessary to correctly load file system start address */
		load_fs_start( (uint32_t)mod->mod_start);

		while(mod_count < mbi->mods_count) {
			printf("Module %d loaded at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_start);
			printf("Module %d ends at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_end);
			printf("First few bytes of module:\n");
			for(i = 0; i<16; i++) {
				printf("0x%x ", *((char*)(mod->mod_start+i)));
			}
			printf("\n");
			mod_count++;
		}
	}
	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
	{
		printf ("Both bits 4 and 5 are set.\n");
		return;
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG (mbi->flags, 5))
	{
		elf_section_header_table_t *elf_sec = &(mbi->elf_sec);

		printf ("elf_sec: num = %u, size = 0x%#x,"
				" addr = 0x%#x, shndx = 0x%#x\n",
				(unsigned) elf_sec->num, (unsigned) elf_sec->size,
				(unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6))
	{
		memory_map_t *mmap;

		printf ("mmap_addr = 0x%#x, mmap_length = 0x%x\n",
				(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
		for (mmap = (memory_map_t *) mbi->mmap_addr;
				(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
				mmap = (memory_map_t *) ((unsigned long) mmap
					+ mmap->size + sizeof (mmap->size)))
			printf (" size = 0x%x,     base_addr = 0x%#x%#x\n"
					"     type = 0x%x,  length    = 0x%#x%#x\n",
					(unsigned) mmap->size,
					(unsigned) mmap->base_addr_high,
					(unsigned) mmap->base_addr_low,
					(unsigned) mmap->type,
					(unsigned) mmap->length_high,
					(unsigned) mmap->length_low);
	}

	/* Construct an LDT entry in the GDT */
	{
		seg_desc_t the_ldt_desc;
		the_ldt_desc.granularity    = 0;
		the_ldt_desc.opsize         = 1;
		the_ldt_desc.reserved       = 0;
		the_ldt_desc.avail          = 0;
		the_ldt_desc.present        = 1;
		the_ldt_desc.dpl            = 0x0;
		the_ldt_desc.sys            = 0;
		the_ldt_desc.type           = 0x2;

		SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
		ldt_desc_ptr = the_ldt_desc;
		lldt(KERNEL_LDT);
	}

	/* Construct a TSS entry in the GDT */
	{
		seg_desc_t the_tss_desc;
		the_tss_desc.granularity    = 0;
		the_tss_desc.opsize         = 0;
		the_tss_desc.reserved       = 0;
		the_tss_desc.avail          = 0;
		the_tss_desc.seg_lim_19_16  = TSS_SIZE & 0x000F0000;
		the_tss_desc.present        = 1;
		the_tss_desc.dpl            = 0x0;
		the_tss_desc.sys            = 0;
		the_tss_desc.type           = 0x9;
		the_tss_desc.seg_lim_15_00  = TSS_SIZE & 0x0000FFFF;

		SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

		tss_desc_ptr = the_tss_desc;

		tss.ldt_segment_selector = KERNEL_LDT;
		tss.ss0 = KERNEL_DS;
		tss.esp0 = 0x800000;
		ltr(KERNEL_TSS);
	}


	/* Construct the IDT */
	{
		idt_desc_t the_idt_desc;


		/*		This is the IDT Gate Descriptor for the Interrupt Gate
		 *
		 *		+31------------+15-+14-13+12--8+7-5+4--0+
		 *		| Offset 31-16 | P | DPL |0D110|000|xxxx|
		 *		+--------------+---+-----+-----+---+----+
		 *
		 *		+31-------------16+15------------------0+
		 *		| Segment Selector| Offset 15-0			|
		 *		+-----------------+---------------------+
		 *
		 *		D = size of gate: 1 = 32 bits, 0 = 16 bits
		*/



		the_idt_desc.reserved4 = 0; /* reserved => 0 */
		the_idt_desc.reserved3 = 0;
		the_idt_desc.reserved2 = 1;
		the_idt_desc.reserved1 = 1;
		the_idt_desc.size = 1;	/* 32-bit segment (addr of where instr is being referenced) */
		the_idt_desc.reserved0 = 0;
		the_idt_desc.dpl = 0; /* the first 19 are in kernel privilege */
		the_idt_desc.present = 1;	/* Segment is available */
		the_idt_desc.seg_selector = KERNEL_CS; /* Selects Kernel code segment */
		
		SET_IDT_ENTRY(the_idt_desc, &wrap_divide_by_zero_exception );
		idt[0] = the_idt_desc;

	/*	Intel Reserved
		idt[1]
	*/
		SET_IDT_ENTRY(the_idt_desc, &wrap_non_maskable_int_exception );
		idt[2] = the_idt_desc;

		SET_IDT_ENTRY(the_idt_desc, &wrap_breakpoint_exception );
		idt[3] = the_idt_desc;

		SET_IDT_ENTRY(the_idt_desc, &wrap_overflow_exception );
		idt[4] = the_idt_desc;

		SET_IDT_ENTRY(the_idt_desc, &wrap_bounds_exceeded_exception );
		idt[5] = the_idt_desc;

		SET_IDT_ENTRY(the_idt_desc, &wrap_invalid_opcode_exception );
		idt[6] = the_idt_desc;

		SET_IDT_ENTRY(the_idt_desc, &wrap_device_not_available_exception );
		idt[7] = the_idt_desc;

		SET_IDT_ENTRY(the_idt_desc, &wrap_double_fault_exception );
		idt[8] = the_idt_desc;

		SET_IDT_ENTRY(the_idt_desc, &wrap_co_cpu_segment_overrun_exception );
		idt[9] = the_idt_desc;

		SET_IDT_ENTRY(the_idt_desc, &wrap_invalid_TSS_exception );
		idt[10] = the_idt_desc;

		SET_IDT_ENTRY(the_idt_desc, &wrap_segment_not_present_exception );
		idt[11] = the_idt_desc;

		SET_IDT_ENTRY(the_idt_desc, &wrap_stack_segment_fault_exception );
		idt[12] = the_idt_desc;

		SET_IDT_ENTRY(the_idt_desc, &wrap_general_protection_exception );
		idt[13] = the_idt_desc;

		SET_IDT_ENTRY(the_idt_desc, &wrap_page_fault_exception );
		idt[14] = the_idt_desc;

	/*	Intel Reserved
		idt[15]
	*/
		SET_IDT_ENTRY(the_idt_desc, &wrap_x87_FPU_error_exception ); /* math fault */
		idt[16] = the_idt_desc;

		SET_IDT_ENTRY(the_idt_desc, &wrap_alignment_check_exception );
		idt[17] = the_idt_desc;

		SET_IDT_ENTRY(the_idt_desc, &wrap_machine_check_exception );
		idt[18] = the_idt_desc;	
		
		SET_IDT_ENTRY(the_idt_desc, &wrap_SIMD_FPU_exception );
		idt[19] = the_idt_desc;	

		/*
		 *	20-31 are Intel Reserved
		 *
		*/

		 /* PIT entry - on Master PIC - entry 0x20 */
		 the_idt_desc.dpl = 3;
		 SET_IDT_ENTRY(the_idt_desc, &wrap_pit_interrupt);
		 idt[0x20] = the_idt_desc;
		 
		 /* Keyboard entry - on Master PIC - entry 0x21 */
		 SET_IDT_ENTRY(the_idt_desc, &wrap_keyboard_interrupt);
		 idt[0x21] = the_idt_desc;

		 /* RTC (real time clock) entry  - slave PIC - entry 0x28 */
		 SET_IDT_ENTRY(the_idt_desc, &wrap_rtc_interrupt);
		 idt[0x28] = the_idt_desc;

		 /*
		  * Enable entry for System Calls - uses entry 0x80 - 128(dec)
		 */
		 the_idt_desc.dpl = 3; /* users can use system calls */
		 the_idt_desc.reserved3 = 1; /* we need to allow interrupts */
		 SET_IDT_ENTRY(the_idt_desc, &sys_call_handler);
		 idt[0x80] = the_idt_desc;


		 /* load the IDT into LDTR register */
		lidt(idt_desc_ptr);

	}

	/* Init the PIC */
	i8259_init();

	/* initialize the file operations array setup */
	init_fops();

	/* Initialize devices, memory, filesystem, enable device interrupts on the
	 * PIC, any other initialization stuff... */

	/* init keyboard */
	keyboard_init();
	/* init rtc */
	rtc_init();
	



	/* Enable interrupts */
	/* Do not enable the following until after you have set up your
	 * IDT correctly otherwise QEMU will triple fault and simple close
	 * without showing you any output */
	
	printf("Enabling Interrupts\n");
	sti();
	clear(1);

	/* Execute the first program (`shell') ... */

	/* TESTING RTC DRIVERS - will write an a to screen at varying rate */
/* 	
	uint32_t counter;
	uint32_t inside_count;
	for(counter = 2; counter <= 1024; counter = counter * 2){
		printf("5 a's at %d Hz\n",counter);
		rtc_write(counter);	// change the rtc frequency
		for(inside_count = 0; inside_count < 6; inside_count++){
			rtc_read();     // wait for interrupt
			putc('a');		// print an each interrupt
		}
		putc('\n');
	}
	rtc_open();
*/

	/* testing terminal driver */
	//int8_t temp_buf[128];
	//int32_t bytes_read;
	//printf("test input:  ");
	//bytes_read = terminal_read(temp_buf, 256);
	//temp_buf[bytes_read - 1] = 0; //change the newline to a null terminator
	//printf("the %d bytes read were: %s\n", bytes_read, temp_buf);	
	//terminal_write("test write to the terminal", 26);
	
	/*
	 *
	 *	Testing file system
	 *
	 *
	*/
/*
	int fd;
	uint8_t buf[BUF_SIZE] = "";
	int i = 0;
	uint32_t x = 0;
*/

	/*
	 *
	 * test read dir
	 *
	*/
/*
	if (-1 == (fd = fs_open((uint8_t*)".")))
	{
        printf("directory open failed\n");
    }
	do
	{
		x = fs_read(buf, 10);
	   	printf("read %d bytes: buf=%s\n",x,buf);
	} while( x != 0 );

	fs_close( (uint8_t *)fd);
*/

	/*
	 * 
	 * test read from text file
	 * 
	*/
/*
	if (-1 == (fd = fs_open((uint8_t*)"frame0.txt")))
	{
        printf("directory open failed\n");
    }

	do
	{
		x = fs_read(buf, 1);
	   	printf("%s",buf);
	} while( x != 0 );

*/

/*
	if (-1 == (fd = fs_open((uint8_t*)"verylargetxtwithverylongname.tx")))
	{
        printf("directory open failed\n");
    }

	do
	{
		x = fs_read(buf, 1);
	   	printf("%s",buf);
	} while( x != 0 );
*/

	/*
	 *
	 * test read from other file
	 *
	*/
/*
	if (-1 == (fd = fs_open((uint8_t*)"fish")))
	{
        printf("directory open failed\n");
    }
    
	x = fs_read(buf, 10);
	printf("read %d bytes\n",x);
	while( i != x )
	{
		printf("i=%d, buf[i]=%c\n",i,buf[i]);
		i++;
	}
*/
	/* initialize terminal instances */
	terminal_initialize_terminals();
	/* init PIT */
	pit_init();

	// login/authenticate user
	authenticate();

	sys_execute(NULL);

	/* Spin (nicely, so we don't chew up cycles) */
	asm volatile(".1: hlt; jmp .1;");
}
