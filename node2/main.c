#include <stdio.h>
#include <stdarg.h>
#include "sam/sam3x/include/sam.h"
//#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "startercode/time.h"

#include "startercode/uart.h"
#include "../can_node_2/can_controller.h"
#include "../can_node_2/can_interrupt.h"


//#include "../node1/mcp2515.h"
//#include <util/delay.h>
//picocom -b 9600 /dev/ttyACM1


/*
 * Remember to update the Makefile with the (relative) path to the uart.c file.
 * This starter code will not compile until the UART file has been included in the Makefile. 
 * If you get somewhat cryptic errors referencing functions such as _sbrk, 
 * _close_r, _write_r, _fstat etc, you have most likely not done that correctly.

 * If you get errors such as "arm-none-eabi-gcc: no such file", you may need to reinstall the arm gcc packages using
 * apt or your favorite package manager.
 */

//acm0
#define F_CPU 84000000 
#define BAUD_RATE 9600
#define BIT_RATE 250000


#define BRP (F_CPU / (BIT_RATE * 16)) -1;
#define phaseseg2 6-1;
#define phaseseg1   phaseseg2+1;
#define propag   2-1;
#define SJW   3;
#define smp 0;


void UART_verification() {
    uart_init(F_CPU, BAUD_RATE);
    printf("\n\rUART is ready on Node 2!\n\r");
}
int main()
{
SystemInit();
UART_verification();


WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

//uint8_t BRP = 20;
//uint8_t phaseseg2=4;
//uint8_t phaseseg1 = 7;
//uint8_t propag = 1;
//uint8_t SJW = 2;
//uint8_t smp=0;

//PMC ->PMC_PCER0 = (1 << ID_PIOB); //starter klokka


uint32_t can_br = ((smp<<24) | (BRP<<16) | (SJW<<12) | (propag<<8) | (phaseseg1<<4) | phaseseg2);
//uint8_t rxInterrupt=1;
can_init_def_tx_rx_mb(can_br);



//#define txMailbox 0
//#define rxMailbox 1
char can_sr = CAN0->CAN_SR; 
CAN_MESSAGE message;
while(1){
//time_spinFor(msecs(200));
	if (!can_receive(&message,0)){
        if (message.id==0x43||message.id==0x01){
            printf("%u\n", (unsigned)message.data[0]); 
        }
    }

//msecs(200);
//time_spinFor(2);
}}

/*__attribute__((packed)) struct CanInit {
    union {
        struct {
            uint32_t phase2:4;  // Phase 2 segment
            uint32_t propag:4;  // Propagation time segment
            uint32_t phase1:4;  // Phase 1 segment
            uint32_t sjw:4;     // Synchronization jump width
            uint32_t brp:8;     // Baud rate prescaler
            uint32_t smp:8;     // Sampling mode
        };
        uint32_t reg;
    };
};
		if(can_sr & CAN_SR_MB1)  //Mailbox 1 event
		{
            can_receive(&message, 1);
            printf("%d \n", message.data);
		}
        
		else if(can_sr & CAN_SR_MB2) //Mailbox 2 event
		{
			can_receive(&message, 2);
            printf("%d \n", message.data);
		}
*/

/*WDT->WDT_MR = WDT_MR_WDDIS;      // Disable Watchdog

PIOA->PIO_PER = PIO_PER_P1;      // Enable PIO control on PA1
PIOA->PIO_OER = PIO_OER_P1;      // Configure PA1 as output

// Test: Sett pin høy, vent litt, så lav
PIOA->PIO_SODR = PIO_SODR_P1;    // Set Output Data Register – pin HIGH
for (volatile int i = 0; i < 1000000; i++);  // enkel delay
PIOA->PIO_CODR = PIO_CODR_P1;    // Clear Output Data Register – pin LOW


    while (1)
    {
    
            // Sett pin høy (servo puls)
    PIOA->PIO_SODR = PIO_SODR_P1;
    //delay_us(1500);   // 1.5 ms for midtposisjon
    // Sett pin lav
    PIOA->PIO_CODR = PIO_CODR_P1;
    //delay_ms(18.5);   // Resten av 20 ms perioden
    }*/
