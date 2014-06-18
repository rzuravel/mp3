/* rtc.h - header file for the RTC driver
*/

#ifndef _RTC_DRIVER_H
#define _RTC_DRIVER_H

#include "types.h"
#include "i8259.h"
#include "lib.h"

/* Ports that RTC sits on */
#define RTC_REG_SELECT 	0x70
#define RTC_READ_WRITE	0x71
#define RTC_IRQ			0x08

/* Register offsets for RTC */
#define RTC_REG_A 		0x0A
#define RTC_REG_B		0x0B
#define RTC_REG_C		0x0C
#define RTC_REG_D		0x0D

/* Values to send to Registers */
#define REG_A_VAL		0x20
#define REG_B_VAL_MASK		0x80
#define REG_B_VAL_SET		0x41
#define TWO_HZ			0x0F

/* Externally-visible functions */

/* "open" the rtc - initialize */
extern int32_t rtc_open(const uint8_t* filename);

/* read from the rtc */
extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);

/* Write a frequency out to rtc */
extern int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);

/* "close" the rtc */
extern int32_t rtc_close(int32_t fd);

/* Handlers for rtc set-up and interrupts */
extern void rtc_init();
extern void rtc_handler();
extern int32_t rtc_checkforint();

#endif /* _RTC_DRIVER_H */
