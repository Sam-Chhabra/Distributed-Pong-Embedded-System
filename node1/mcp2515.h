#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <util/delay.h>
#include "spi.h"
#include <stdio.h>

#define INT PD2
#define MCP_READ 0x03
#define MCP_WRITE 0x02
#define MCP_CANSTAT 0x0E
#define MODE_MASK 0xE0
#define MODE_CONFIG 0x80
#define MCP_TX0 0x81
#define MCP_TX1 0x82
#define MCP_TX2 0x84
#define MCP_BIT_MODIFY 0b00000101
#define MCP_RESET 0b11000000
#define MCP_READ_STATUS 0b10100000
#define MCP_CANCTRL		0x0F
#define MODE_LOOPBACK   0x40

uint8_t mcp2515_init();
uint8_t mcp2515_read(uint8_t address);
void mcp2515_write(uint8_t address, uint8_t data);
void mcp2515_request_to_send(int buffer_index);
void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data);
void mcp2515_reset();
uint8_t mcp2515_read_status();
void mcp2515_set_mode(uint8_t mode);