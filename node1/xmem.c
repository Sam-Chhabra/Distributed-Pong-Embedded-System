#include "xmem.h"

void test_latch(void){
  //set ale as output
  DDRE = 0b10;
  PORTE = 0b10; // set ale hight. a adress is coming to be stored
  PORTA = 0b000001; // = 0x00. send adress
  _delay_ms(2000); 
  PORTE = 0b00; // ale low. the adress is stored inthe latch
  _delay_ms(2000);
  PORTA = 0b01010101; //send new adress
  _delay_ms(2000);
  PORTE = 0b10; // old adress is removed from latch

}
