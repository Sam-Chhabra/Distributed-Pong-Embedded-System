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
#include "startercode/PWM.h"


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
#define BIT_RATE 125000
#define MCK 42000000 

//#define BRP ((MCK / (BIT_RATE * 16)) )
//#define phaseseg2 (6)
//#define phaseseg1   (phaseseg2+1)
//#define propag   (2)
//#define SJW   1
//#define smp 0

#define BRP ((F_CPU/2000000) -1)
#define phaseseg2 1
#define phaseseg1   5
#define propag   6
#define SJW   0
#define smp 0

void delay_ms(uint16_t tid){
    for (int i=0;i<tid*100;i++){
        __asm__("nop");
    }
}

void can_printmsg1(CAN_MESSAGE *m){
    printf("CanMsg(id:%d, length:%d, data:{", m->id, m->data_length);
    
    for(uint8_t i = 0; i < m->data_length; i++){
        printf("%c", m->data[i]);
    }
    printf("})\n");
}

void UART_verification() {
    uart_init(F_CPU, BAUD_RATE);
    printf("\n\rUART is ready on Node 2!\n\r");
}

void dag6_test(){
        CAN_MESSAGE message;
while(1){
        if (!can_receive(&message, 0)){
            can_printmsg1(&message);
        }
    }
}

void dag7_test(){
    pwm_init();
    pwm_set_duty_cycle(20);
}



int main()
{
    SystemInit();
    UART_verification();
    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer
    uint32_t can_br = ((smp<<24) | (BRP<<16) | (SJW<<12) | (propag<<8) | (phaseseg1<<4) | phaseseg2);
    can_init_def_tx_rx_mb(can_br);
    dag7_test();
    CAN_MESSAGE message;
    while(1){
            if (!can_receive(&message, 0)){
                duty(&message);
                //delay_ms(100000);
            }
    }
}


