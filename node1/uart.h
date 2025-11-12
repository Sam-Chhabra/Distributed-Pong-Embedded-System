#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/interrupt.h>

void UART_init(unsigned int ubrr);
int UART_transmit(char data, FILE *file);
int UART_receive(FILE * file);
