#ifndef DRIVERS_SERIAL_H_
#define DRIVERS_SERIAL_H_

#include <stdarg.h>
#include <types.h>
#include <stdlib.h>

#define PORT_COM1 0x3f8

#define RECEIVE_BUFFER 0
#define TRANSMIT_BUFFER 0
#define INTERRUPT_REGISTER 1
#define BAUDRATE_LEAST_SIGNIFANT 0
#define BAUDRATE_MOST_SIGNIFANT 1
#define LINE_CONTROL_REGISTER 3
#define FIFO_CONTROL_REGISTER 2
#define MODEM_CONTROL_REGISTER 4

#define LINE_STATUS_REGISTER 5

#define DISABLE_INTERRUPTS 0x00
#define ENABLE_DLAB 0x80
#define STOP_BIT 0b000
#define NO_PARITY 0
#define BITS_8 0b11

#define INT_THRESHOLD_14 (0b11 << 6)
#define CLEAR_TRANSMIT_FIFO (0b1 << 2)
#define CLEAR_RECEIVE_FIFO (0b1 << 1)
#define ENABLE_FIFO (0b1)

#define DATA_TERMINAL_READY 0b1
#define REQUEST_TO_SEND 0b10
#define ENABLE_IRQ 0b1000

#define TRANSMITTER_HOLDING_REGISTER_EMPTY 0x20

extern int serial_init();
extern void serial_write(char ch);
extern void serial_writeline(char *str);
extern void serial_write_str(char *str);
extern void printfs(const char *format, ...);

#endif // DRIVERS_SERIAL_H_