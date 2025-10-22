#include "xmem.h"

void test_latch(void){
  DDRE = 0b10; // Setter ALE (pin 1 på port E) som utgang. 0=inngang, 1=utgang
  PORTE = 0b10; // Setter ALE høy. Forteller vippa at nå kommer det en adresse som skal lagres.
  PORTA = 0b000001; // = 0x00. Sender en adresse ut til vippa.
  _delay_ms(2000); // 2 sek delay for å gjøre det enklere å måle på kretsen.
  PORTE = 0b00; // Setter ALE lav. Nå lagres adresseverdien i vippa.
  _delay_ms(2000);
  PORTA = 0b01010101; // Sender ut ny adresse. Ingenting skjer på baksiden av vippa.
  _delay_ms(2000);
  PORTE = 0b10; // Gammel adresse fjernes fra vippa og den nye sendes gjennom.

}
