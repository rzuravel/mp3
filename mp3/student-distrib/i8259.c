/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* Initialize the 8259 PIC
 * inputs: none
 * outputs: none
 */
void 
i8259_init(void)
{
	printf("Starting PIC initialization\n");

	/* init master interrupt controller */
    outb(ICW1, MASTER_8259_PORT); 		/* Start init sequence */
    outb(ICW2_MASTER, MASTER_DATA); 	/* Vector base */
    outb(ICW3_MASTER, MASTER_DATA); 	/* edge tiggered, Cascade (slave) on IRQ2 */

    outb(ICW4, MASTER_DATA); 			/* Select 8086 mode */
  
    /* init slave interrupt controller */
    outb(ICW1, SLAVE_8259_PORT); 		/* Start init sequence */
    outb(ICW2_SLAVE, SLAVE_DATA); 		/* Vector base */
    outb(ICW3_SLAVE, SLAVE_DATA); 		/* edge triggered, Cascade (slave) on IRQ2 */
    outb(ICW4, SLAVE_DATA); 			/* Select 8086 mode */
	
	/* Mask all interrupts */
	master_mask = 0xFF;
	slave_mask = 0xFF;
    outb(master_mask, MASTER_DATA);
    outb(slave_mask, SLAVE_DATA);
 
	printf("Done with PIC initialization\n");
 
}

/* Enable (unmask) the specified IRQ
 * inputs: irq_num to enable
 * outputs: none
 */
void
enable_irq(uint32_t irq_num)
{
	uint8_t mask = ~(1 << (irq_num % 8));	// sets the correct bit to be 0
	
	/* change the corresponding irq bit to 0 */
	if(irq_num < 8) master_mask &= mask;
	else if(irq_num < 16){
		slave_mask &= mask;
		master_mask &= ~(1 << 2);
	}
	
	/* use the corresponding masks to mask IRQ's */
	outb(master_mask,MASTER_DATA);
	outb(slave_mask,SLAVE_DATA);
}

/* Disable (mask) the specified IRQ
 * inputs: irq_num to disable
 * outputs: none
 */
void
disable_irq(uint32_t irq_num)
{
	uint8_t mask = 1 << (irq_num % 8);	// sets the correct bit to be 1
	
	/* change the corresponding irq bit to 1 */
	if(irq_num < 8) master_mask |= mask;
	else  if(irq_num < 16) slave_mask |= mask;
	
	/* use the corresponding masks to unmask IRQ and continue masking others */
	outb(master_mask,MASTER_DATA);
	outb(slave_mask,SLAVE_DATA);
}

/* Send end-of-interrupt signal for the specified IRQ
 * input: irq_num or interrupt that has ran
 * outputs: none
 */
void
send_eoi(uint32_t irq_num)
{
	uint8_t eoi_num;
	eoi_num	= EOI | irq_num;	// OR'd value of EOI and irq_num creates the wanted eio signal
	if(irq_num > 7){
		eoi_num	= EOI | (irq_num % 8);	
		outb(eoi_num,SLAVE_8259_PORT);	// send the 0-8 EOI to the Slave
		eoi_num	= EOI | 2;
		outb(eoi_num,MASTER_8259_PORT);	// send the 2 EOI to the Master
	}
	else outb(eoi_num,MASTER_8259_PORT);
}

