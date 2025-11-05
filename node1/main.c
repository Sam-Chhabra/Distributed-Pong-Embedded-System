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
// cd Desktop/gr12/ByggernLab

int main(){
    _delay_ms(500);
    UART_init(MYUBRR);
    fdevopen(UART_transmit, UART_receive);
    SRAM_init();
    adc_init();
    spi_init();
    spi_deselectSlave();
    
    //can_normal_init();
    CAN_int_init_PD2();
    can_normal_init();
  
    can_message message = {
		2, // Id
		5, // Lengde på dataen
		"heiii"//data. Maks åtte byte
	};
    adc_values_t cal_data;
    pos_calibrate(&cal_data); //kalibrerer
    adc_values_t pos;
    while(1){
        position(cal_data.joystick_x, cal_data.joystick_y, &pos);
        //can_send(&message,0); // Sender melding
        //printf("%d",message.data[0]);
        //can_send1(&message,1);
        _delay_ms(20);
        send_pos(&pos);
        _delay_ms(100);
    }
        
	return 0;



    /*
    
    _delay_ms(200);
    //mcp2515_set_mode(MODE_NORMAL); 
    CAN_int_init_PD2();   
    //CAN_init_normal();
    //printf("hei");
    //send_joystick_pos();
        can_message tx_message = {
            .id = 0x01,
            .data_length = 1,             
            .data = {0x01}
        };
    while(1){
        //printf("txb %d \n",mcp2515_read(MCP_TXB0CTRL));
        //printf(" txreq %d \n", MCP_TXREQ_BIT);
//_delay_ms(2000);
    //if (!(mcp2515_read(MCP_TXB0CTRL) & MCP_TXREQ_BIT)){
        //printf("tom");  
            //send_joystick_pos();
        _delay_ms(100);
        can_send(&tx_message,1);
    }
    */
   // }


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

    return 0;
    can_message tx = { 
    .id = 1, 
    .data_length = 4, 
    .data = {"woah"} };
uint16_t inflight_id = 0;
uint8_t  tx_inflight = 0;

for(;;){
    _delay_ms(600);
    CAN_service();
    if (!tx_inflight) {
        if (can_try_send(&tx)) {
            inflight_id = tx.id;    
            tx_inflight = 1;         
        }
    }
    can_message rx;
    if (CAN_try_get(&rx)) {
        
        uint8_t len = rx.data_length > 8 ? 8 : rx.data_length;
        char msg[9]; for (uint8_t i=0;i<len;i++) msg[i]=(char)rx.data[i]; msg[len]='\0';

        printf("Id: %u\r\nLength: %u\r\nMessage: %s\r\n\r\n",
               (unsigned)rx.id, rx.data_length, msg);
        tx.id = (inflight_id + 1) & 0x7FF;
        tx_inflight = 0;
    }

}*/