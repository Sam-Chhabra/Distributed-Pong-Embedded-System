#include <stdio.h>
#include <stdarg.h>
#include "sam/sam3x/include/sam.h"
//#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "startercode/time.h"
//#include <util/delay.h>
//picocom -b 115200 /dev/ttyACM1


/*
 * Remember to update the Makefile with the (relative) path to the uart.c file.
 * This starter code will not compile until the UART file has been included in the Makefile. 
 * If you get somewhat cryptic errors referencing functions such as _sbrk, 
 * _close_r, _write_r, _fstat etc, you have most likely not done that correctly.

 * If you get errors such as "arm-none-eabi-gcc: no such file", you may need to reinstall the arm gcc packages using
 * apt or your favorite package manager.
 */
#include "startercode/uart.h"
//acm0
#define F_CPU 84000000
#define BAUD_RATE 9600

void UART_verification() {
    uart_init(F_CPU, BAUD_RATE);
    printf("\n\rUART is ready on Node 2!\n\r");
}
int main()
{
SystemInit();
UART_verification();

WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer


PMC ->PMC_PCER0 = (1 << ID_PIOB); //starter klokka
//PIOB->PIO_PER |= PIO_PB12; 
PIOB->PIO_OER |= PIO_PB12; //output enble
PIOB->PIO_SODR |= PIO_PB12; //sette høy

time_spinFor(msecs(2000));

PIOB->PIO_CODR |= PIO_PB12; //lav

time_spinFor(msecs(2000));

PIOB->PIO_SODR |= PIO_PB12; //høy

time_spinFor(msecs(2000));

PIOB->PIO_CODR |= PIO_PB12; //lav

}


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