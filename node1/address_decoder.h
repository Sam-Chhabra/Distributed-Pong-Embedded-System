#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "uart.h"
#include "sram.h"

void dec_test(void);
static void burst_range(uint16_t start, uint16_t end) ;
static inline void poke(uint16_t addr, uint8_t val);