/* rtc.c - File for the RTC driver
*/

#include "rtc.h"


/* Global variables */
int8_t int_flag;


/* "open" the rtc
* inputs: none
* outputs: returns 0
* side effects: sets rtc to 2 Hz
*/
int32_t rtc_open(const uint8_t* filename)
{
	cli();
	outb(RTC_REG_A, RTC_REG_SELECT);
	outb(REG_A_VAL | TWO_HZ, RTC_READ_WRITE); // set oscillator on and interrupt set to 2 Hz
	sti();
	return 0;
}

/* read from the rtc
* inputs: none 
* outputs: returns 0 (success) after an rtc interrupt
*/
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
{
	int_flag = 1;
	while(rtc_checkforint()); //when interrupt occurs, int_flag is set to 1
	return 0; //should never reach this anyway
}

/* Write the interrupt rate to the rtc
* inputs: four byte integer specifying rate in Hz
* outputs: 0 on sucess, -1 on fail (invalid frequency)
* side effects: writes characters to the screen
*/
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes)
{
	int8_t rate;
	if (*(uint32_t *)buf == 2) rate = 0x0F;
	else if (*(uint32_t *)buf == 4) rate = 0x0E;
	else if (*(uint32_t *)buf == 8) rate = 0x0D;
	else if (*(uint32_t *)buf == 16) rate = 0x0C;
	else if (*(uint32_t *)buf == 32) rate = 0x0B;		// set register values to corresponding frequency
	else if (*(uint32_t *)buf == 64) rate = 0x0A;
	else if (*(uint32_t *)buf == 128) rate = 0x09;
	else if (*(uint32_t *)buf == 256) rate = 0x08;
	else if (*(uint32_t *)buf == 512) rate = 0x07;
	else if (*(uint32_t *)buf == 1024) rate = 0x06;
	else {
		return -1;
	}
	cli();
	outb(RTC_REG_A, RTC_REG_SELECT);
	outb(REG_A_VAL | rate, RTC_READ_WRITE);		// set oscillator on and interrupt set to correct frequency
	sti();
	return 0;
}

/* "close" the rtc
* inputs: none
* outputs: returns 0 on success (always)
* side effects: none
*/
int32_t rtc_close(int32_t fd)
{
	return 0;
}

/*
 * rtc_init
 *   DESCRIPTION: Initialize the RTC to send interrupts to the PIC at 2 Hz
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Sets frequency to 2 Hz
 */ 
void rtc_init()
{
	cli();
	outb(RTC_REG_B,RTC_REG_SELECT);	
	outb(REG_B_VAL_MASK,RTC_READ_WRITE);		// prepare RTC to accept new values into registers

	outb(RTC_REG_B,RTC_REG_SELECT);
	outb(REG_B_VAL_SET,RTC_READ_WRITE);		// set periodic interrupt mode on and DSE on
	
	outb(RTC_REG_A,RTC_REG_SELECT);
	outb(REG_A_VAL | TWO_HZ,RTC_READ_WRITE);		// set oscillator on and interrupt set to 2 Hz
	
	enable_irq(RTC_IRQ); /* Enable RTC interrupt and slave PIC */
	sti();
}

/*
 * rtc_handler
 *   DESCRIPTION: Called by wrapper when IRQ 8 occurs
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: grabs a byte from the rtc
 */ 
void rtc_handler()
{
	uint8_t prev;
	cli();
	outb(RTC_REG_C,RTC_REG_SELECT);
	prev = inb(RTC_READ_WRITE);		// reset register C and interrupt
	int_flag = 0;
	send_eoi(RTC_IRQ);
	sti();
}

/*
 * rtc_checkforint
 *   DESCRIPTION: Called by rtc_read to look for an updated int_flag
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 1 on no interrupt and 0 on no interrupt having occurred
 *	 SIDE EFFECTS: none
 */ 
int32_t rtc_checkforint()
{
	if(int_flag == 0) return 0;
	else return 1;
}
