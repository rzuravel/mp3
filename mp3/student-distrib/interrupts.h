#ifndef _KEYBOARD_HANDLER_H
#define _KEYBOARD_HANDLER_H

#include "types.h"
#include "i8259.h"
#include "lib.h"
#include "terminal_driver.h"
#include "login.h"



/* Used to read data from and write data to keyboard encoder (port) */
#define KEYBOARD_READ_INPUT_BUFFER_PORT 0x60
#define KEYBOARD_ENC_WRITE_PORT 0x60

/* Commands sent to keyboard encoder - did not use, but we may need later */
#define KEYBOARD_ENC_ENABLE 0xF4
#define KEYBOARD_ENC_RESET_WAIT 0xF5 /* Reset to power on condition and wait for enable command */
#define KEYBOARD_ENC_RESET_ENABLE 0xF6 /* Reset to power on condition and begin scanning keyboard */


/* Used to read status or send command byte to keyboard controller (port) */
#define KEYBOARD_READ_STATUS_REG_PORT 0x64
#define KEYBOARD_CTL_WRITE_PORT 0x64

/* Commands sent to keyboard controller */
#define KEYBOARD_CMD_SET_LED 0xED /* Send this, then send data byte */
#define KEYBOARD_CMD_ENABLE 0xAE
#define KEYBOARD_CMD_DISABLE 0xAD
#define KEYBOARD_CMD_READ_OUTPUT 0xD0 /* Send this, then read from port 0x64 */

/* Commands read from keyboard controller */
#define KEYBOARD_CMD_SET_LED_ACK 0xFA /* ready for led data */

/* Data sent from driver */
#define KEYBOARD_DATA_LEDS_OFF 0x0

#define KEYBOARD_IRQ 0x01

/* Extra */
#define SHIFT_OFFSET 0x3A
#define TAB 0x0F
#define LEFT_BRACE 0x01A
#define CTRL_LEFT 0x01D
#define SEMICOLON 0x027
#define BACKSLASH 0x02B
#define COMMA 0x033


/* Externally-visible functions */

/* Handlers for keyboard set-up and interrupts */
extern void keyboard_init();
extern void keyboard_handler();
extern uint8_t get_scancode(uint8_t letter);



#endif
