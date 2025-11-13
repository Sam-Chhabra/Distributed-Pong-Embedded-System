#include "solenoid.h"
#include "../sam/sam3x/include/sam.h"
#include "../sam/sam3x/include/sam3x8e.h"

#define SOLENOID_PIN 16

void solenoid_init(){ //nr 2 på digital 27
    PMC->PMC_PCER0 |= (1 << 11);
    REG_PIOA_PER |= (1 << SOLENOID_PIN); //enables the pio to control the corresponding pin
    REG_PIOA_OER |= (1<<SOLENOID_PIN); //enable output
    REG_PIOA_SODR |= (1<< SOLENOID_PIN); //set as retract
}

void solenoid_out(){ //kick

    REG_PIOA_CODR |= (1<< SOLENOID_PIN);
}

void solenoid_in() { //retract
    REG_PIOA_SODR |= (1<< SOLENOID_PIN);
   
}

