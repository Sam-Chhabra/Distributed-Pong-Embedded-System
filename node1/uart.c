#include "uart.h"

void UART_init(unsigned int ubrr)
{   
    // Set baud rate
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    
    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
    // Set frame format: 8 data bits, 2 stop bit, no parity
    UCSR0C = (1 << URSEL0) | (1<<USBS0) | (3<<UCSZ00);
    //fdevopen(USART_Transmit, USART_Receive);
}

int UART_transmit(char data, FILE *file)
{
    // Wait for the transmit buffer to be empty
    if ( data == '\n')
       UART_transmit('\r', file);
    while (!(UCSR0A & (1<<UDRE0))) 
    ;
    
    // Put data into the buffer, sends the data
    
    UDR0 = data; 
    return 0;
}

/*UBRR
uint8_t uart_TryTransmit(unsigned chat data){
  if((UCSRA & (1 << UDRE))){ //when its ready, its emty
    UDR = data;
    return 1;
  }
  return 0;
}
*/

int UART_receive(FILE * file)
{
    // Wait for data to be received
    while (!(UCSR0A & (1<<RXC0)))  {
        ;
    } 
    
    // Get and return the received data from the buffer
    return UDR0;
}

/*
int UART_putchar(char c, FILE *stream)
{
  if (c == '\n'){
    UART_transmit('\r');
  }
  UART_transmit((uint8_t)c);
  return 0;
}

int UART_getchar(void){
  return UART_receive();
}

void UART_test_receive_transmit(){
    while(1){
        unsigned char data = UART_receive();
        UART_transmit(data);
    }
}
*/