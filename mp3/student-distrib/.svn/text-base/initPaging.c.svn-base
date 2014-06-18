/* initPaging.c - Functionto initialize paging for the OS
 */

#include "initPaging.h"
#include "lib.h"

void pde_set_bit(uint32_t *pde, uint32_t attrib);
void pde_clear_bit(uint32_t *pde, uint32_t attrib);
void pde_set_base(uint32_t *pde, uint32_t addr);
void pte_set_bit(uint32_t *pte, uint32_t attrib);
void pte_clear_bit(uint32_t *pte, uint32_t attrib);
void pte_set_base(uint32_t *pte, uint32_t addr);


/* Initialize paging
 * inputs: none
 * outputs: none
 * side effects: enables paging, writes to CR0, CR3, CR4
 */
void 
initPaging(void)
{
	int pid_num, entry_num;
	
	for (pid_num = 0; pid_num < MAX_PROCESSES;  pid_num++) {
		
		/*   set up page directory   */
		// set up video mem and kernel entries
	
		//clear page dir
		for (entry_num = 0; entry_num < TABLES_PER_DIR; entry_num++) {
			pdir[pid_num].pde[entry_num] = 0;
		}
		//set first entry (0-4MB)
		pde_set_bit(&pdir[pid_num].pde[PHYS_VID_PDE], I86_PDE_PRESENT);
		pde_set_bit(&pdir[pid_num].pde[PHYS_VID_PDE], I86_PDE_WRITABLE);
		pde_set_base(&pdir[pid_num].pde[PHYS_VID_PDE], (uint32_t)pt_phys_vid[pid_num].pte);
		//set second entry (kernel 4 - 8MB)
		pde_set_bit(&pdir[pid_num].pde[KERNEL_PDE], I86_PDE_PRESENT);
		pde_set_bit(&pdir[pid_num].pde[KERNEL_PDE], I86_PDE_WRITABLE);
		pde_set_bit(&pdir[pid_num].pde[KERNEL_PDE], I86_PDE_4MB);
		pde_set_base(&pdir[pid_num].pde[KERNEL_PDE], KERNEL_START_ADDR);
		//set user video memory entry (64 - 68MB)
		pde_set_bit(&pdir[pid_num].pde[USER_VID_PDE], I86_PDE_PRESENT);
		pde_set_bit(&pdir[pid_num].pde[USER_VID_PDE], I86_PDE_WRITABLE);
		pde_set_bit(&pdir[pid_num].pde[USER_VID_PDE], I86_PDE_USER);
		pde_set_base(&pdir[pid_num].pde[USER_VID_PDE], (uint32_t)pt_virt_vid[pid_num].pte);
		//set user (32nd) entry (128 - 132MB) maps to 8MB, 12MB, 16MB... so on, based on pid_num
		pde_set_bit(&pdir[pid_num].pde[USER_PDE], I86_PDE_PRESENT);
		pde_set_bit(&pdir[pid_num].pde[USER_PDE], I86_PDE_WRITABLE);
		pde_set_bit(&pdir[pid_num].pde[USER_PDE], I86_PDE_USER);
		pde_set_bit(&pdir[pid_num].pde[USER_PDE], I86_PDE_4MB);
		pde_set_base(&pdir[pid_num].pde[USER_PDE], USER_START_ADDR + USER_SPACE_LENGTH * pid_num );

		 
		/*   set up page tables   */
		//set up pt_phys_vid (for 0-4MB)
		//pt_phys_vid.pte[0] = 0;		//clear pte 0 (0 - 4KB, leave it blank)
		/* map the rest of the page table */
		for (entry_num = 0; entry_num < PAGES_PER_TABLE; entry_num++) {
			pt_phys_vid[pid_num].pte[entry_num] = 0;		//clear pte
			/* identity map kernel video memory */
			//if (entry_num >= (VIDEO / PAGE_SIZE) || entry_num <= (VIDEO / PAGE_SIZE) * VID_NUM_PAGES) {
			if (entry_num > 0) {
				pte_set_bit(&pt_phys_vid[pid_num].pte[entry_num], I86_PTE_PRESENT);
				pte_set_bit(&pt_phys_vid[pid_num].pte[entry_num], I86_PTE_WRITABLE);
				pte_set_base(&pt_phys_vid[pid_num].pte[entry_num], entry_num * PAGE_SIZE);
			}
		}
		
		//set up pt_virt_vid (for 64-68MB)
		/* map page table */
		for (entry_num = 0; entry_num < PAGES_PER_TABLE; entry_num++) {
			pt_virt_vid[pid_num].pte[entry_num] = 0;		//clear pte
			/* map to physical video memory (initially) */
			if (entry_num == 0 || entry_num == 1) {
				pte_set_bit(&pt_virt_vid[pid_num].pte[entry_num], I86_PTE_PRESENT);
				pte_set_bit(&pt_virt_vid[pid_num].pte[entry_num], I86_PTE_WRITABLE);
				if (entry_num == 0) {
					pte_set_bit(&pt_virt_vid[pid_num].pte[entry_num], I86_PTE_USER);
					pte_set_base(&pt_virt_vid[pid_num].pte[entry_num], VIDEO);   // + (entry_num * PAGE_SIZE));
				}
				if (entry_num == 1) {
					pte_set_base(&pt_virt_vid[pid_num].pte[entry_num], VIDEO);
				}
			}
		}
	}
	
	
	//set CR3, PSE, and PG
	//set CR3
	setCR3(0); //initial PID will be 0
	//set PSE
	asm volatile ("movl %%cr4, %%eax; bts $4, %%eax; movl %%eax, %%cr4" : : : "%eax");
	//set PG
	asm volatile ("movl %%cr0, %%eax; bts $31, %%eax; movl %%eax, %%cr0" : : : "%eax");

	
	printf("Paging Enabled\n");
}

/* Set CR3 on a context switch
 * inputs: PID -- the process ID that we want to switch tables to
 * outputs: none
 * side effects: writes to CR3
 */
void
setCR3(int PID)
{
		asm volatile   (
				"movl %0, %%eax;"
				"movl %%eax, %%cr3;"
				: 						/*no output*/
				: "r"(pdir[PID].pde)    /* input */
				:"%eax" 			/* clobbered register */
				);
	
}

/* map video memory pages to a back buffer
 * inputs: PID -- the process ID
 *		   terminal_index -- the index of the terminal
 * outputs: none
 * side effects: changes video memory
 */
void set_pid_invisible(uint32_t PID, uint32_t terminal_index)
{
	/* map "physical" (kernel) video address to the back buffer */
	pte_set_base(&pt_phys_vid[PID].pte[VIDEO / PAGE_SIZE], (BACK_BUFFER+terminal_index*PAGE_SIZE) );
	/* map virtual (user) video address to the back buffer */
	pte_set_base(&pt_virt_vid[PID].pte[0], (BACK_BUFFER+terminal_index*PAGE_SIZE));
}


/* map video memory pages back to physical memory
 * inputs: PID -- the process ID
 * outputs: none
 * side effects: changes video memory
 */
void set_pid_visible(uint32_t PID)
{
	/* map "physical" (kernel) video address to physical video */
	pte_set_base(&pt_phys_vid[PID].pte[VIDEO / PAGE_SIZE], VIDEO);
	/* map virtual (user) video address to physical video */
	pte_set_base(&pt_virt_vid[PID].pte[0], VIDEO);
}


/* sets a bit in a pde
 * inputs: 	*pde - a pointer to a pde
 *          attrib - which bit to set
 * outputs: none
 */
void pde_set_bit(uint32_t *pde, uint32_t attrib)
{
	*pde |= attrib;
}

/* clears a bit in a pde
 * inputs: 	*pde - a pointer to a pde
 *          attrib - which bit to clear
 * outputs: none
 */
void pde_clear_bit(uint32_t *pde, uint32_t attrib)
{
	*pde &= ~attrib;
}

/* sets a base address for a pde
 * inputs: 	*pde - a pointer to a pde
 *          addr - what address to use for the base
 * outputs: none
 */
void pde_set_base(uint32_t *pde, uint32_t addr)
{
	*pde &= ~I86_PDE_BASE;
	*pde |= (addr & I86_PDE_BASE);
}

/* sets a bit in a pte
 * inputs: 	*pte - a pointer to a pte
 *          attrib - which bit to set
 * outputs: none
 */
void pte_set_bit(uint32_t *pte, uint32_t attrib)
{
	*pte |= attrib;
}

/* clears a bit in a pte
 * inputs: 	*pte - a pointer to a pte
 *          attrib - which bit to clear
 * outputs: none
 */
void pte_clear_bit(uint32_t *pte, uint32_t attrib)
{
	*pte &= ~attrib;
}

/* sets a base address for a pte
 * inputs: 	*pte - a pointer to a pte
 *          addr - what address to use for the base
 * outputs: none
 */
void pte_set_base(uint32_t *pte, uint32_t addr)
{
	*pte &= ~I86_PTE_BASE;
	*pte |= (addr & I86_PTE_BASE);
}
