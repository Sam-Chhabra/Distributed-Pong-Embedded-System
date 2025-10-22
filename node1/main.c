// main.c
#define F_CPU 4915200UL
#define BAUD_RATE 9600
#define MYUBRR (F_CPU/16/BAUD_RATE-1)

#include "uart.h"
#include "xmem.h"
#include "sram.h"
#include "address_decoder.h"
#include "adc.h"
#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <util/delay.h>
#include "spi.h"
#include "oled.h"
#include "menu.h"
#include "user_io_board.h"
#include "mcp2515.h"
#include "can.h"

// picocom -b 9600 /dev/ttyS0

int main(void){
    UART_init(MYUBRR);
    fdevopen(UART_transmit, UART_receive);
    SRAM_init();
    adc_init();
    spi_deselectSlave();
    user_io_init();
    can_init();
    _delay_ms(200);
    mcp2515_set_mode(MODE_LOOPBACK);  
   can_message tx_message = {
        .id = 1,
        .data_length = 8,             
        .data = "helloooooo"
    };

    can_message rx_message;

    while (1) {
        _delay_ms(100);
        can_try_send(&tx_message);
        can_data_receive(&rx_message);
        uint8_t len = rx_message.data_length;
        if (len > 8) len = 8;          // safety
        char msg[9];                   
        for (uint8_t i = 0; i < len; i++) msg[i] = (char)rx_message.data[i];
        msg[len] = '\0';

        printf("Hello! We received a message.\r\n");
        printf("Id: %u \r\n", (unsigned)rx_message.id);
        printf("Length: %u \r\n", rx_message.data_length);
        printf("Message: %s \r\n\r\n", msg);
        tx_message.id = (tx_message.id + 1) & 0x07FF;
        
        _delay_ms(1000);
    }

    return 0;

  

}

/*    can_message tx_message = {
        .id = 1,
        .data_length = 5,             
        .data = "helloooooo"
    };

    can_message rx_message;

    while (1) {
        tx_message.id = (tx_message.id + 1) & 0x07FF;
        can_try_send(&tx_message);
        can_data_receive(&rx_message);
        uint8_t len = rx_message.data_length;
        if (len > 8) len = 8;          // safety
        char msg[9];                   
        for (uint8_t i = 0; i < len; i++) msg[i] = (char)rx_message.data[i];
        msg[len] = '\0';

        printf("Hello! We received a message.\r\n");
        printf("Id: %u \r\n", (unsigned)rx_message.id);
        printf("Length: %u \r\n", rx_message.data_length);
        printf("Message: %s \r\n\r\n", msg);
        _delay_ms(1000);
    }

    return 0;*/