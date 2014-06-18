/* pit.h - header file for the PIT driver
*/

#ifndef _PIT_DRIVER_H
#define _PIT_DRIVER_H

#include "types.h"
#include "i8259.h"
#include "lib.h"

/* Ports that PIT sits on */
#define PIT_CHANNEL_0 	0x40
#define PIT_CHANNEL_1	0x41
#define PIT_CHANNEL_2   0x42
#define PIT_MODE_REG	0x43

#define PIT_IRQ			0x00

/* Values to send to Registers */
#define MODE_VAL		0x03
#define SET_HZ			0x36
#define HZ_SET			0x5D37

/* Externally-visible functions */

/* Handlers for pit set-up and interrupts */
extern void pit_init();
extern void pit_handler();



#endif /* _PIT_DRIVER_H */
