#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <util/delay.h>

#define IO_SS1 PB4
#define DISP_SS2 PB3
#define MOSI PB5
#define MISO PB6
#define SCK PB7
#define MCP_SS PB1


void spi_init();
void spi_write(uint8_t byte);
uint8_t spi_read();
void spi_transferBytes(const uint8_t *tx_data, uint8_t *rx_data, uint8_t len);
void spi_selectSlave(uint8_t ss);
void spi_deselectSlave();
uint8_t spi_txrx(uint8_t data);