
#include "../sam/sam3x/include/sam.h"
#include "../sam/sam3x/include/sam3x8e.h"

#include <stdio.h>
#include "PWM.h"

//velger metode2
//REG_PWM_CPDRUPDx setter periode verdi

void pwm_init(){  
    PMC->PMC_PCER0 |= (1 << 12);   // PIOB controller b, trenger den til pb13 hvor servoen ligger 
    PMC->PMC_PCER1 |= PMC_PCER1_PID36;   // PWM controller


    PIOB->PIO_PDR |= PIO_PDR_P13; // slår av gpio slik at pinnen kan brukes av pwm
    PIOB->PIO_ABSR |= PIO_ABSR_P13;
    //PIOB->PIO_ABSR |= PIO_PB13B_PWMH0;   // velge B fordi pb13 ligger på periphial B ( A = 0, B = 1)

    // Set up PWM clock
    PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(84);   // klokke A blir 1 MHZ, PREA, DIVA, teller hvert mikrosek
    // f_PWMCLK = MCK / DIVA = 84 MHz / 84 = 1 MHz

   
    PWM->PWM_CH_NUM[1].PWM_CMR = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;   // channel 1 er servo, skal bruke klokke A (CLKA), usikker på CPOL kan teste med og uten

    /*
     * CPRD  = f_PWMCLK / f_signal
     * CDTY  = CPRD × duty
     */

    // Set period and duty cycle
    PWM->PWM_CH_NUM[1].PWM_CPRD = PWM_CPRD_CPRD(20000);  // setter CPRD som gir riktig periode, CPRD*DICA/MCK skal bli perioden på 20 ms
    PWM->PWM_CH_NUM[1].PWM_CDTY = PWM_CDTY_CDTY(1500);   // setter start duty cycle på channel 0, 1.5 ms 

    // Activate channel
    PWM->PWM_ENA = PWM_ENA_CHID1;   // aktverer klokken på kanal 0

}
/*
void pwm_init_motor(){ //  LAB DAG 8
    pwm_init_servo();
    PMC->PMC_PCER0 |= PMC_PCER0_PID12;   // paralell io controller b
    PMC->PMC_PCER1 |= PMC_PCER1_PID36;   // PWM controller

    // Deactivate PIO for PB12
    PIOB->PIO_PDR |= PIO_PDR_P12;
    PIOB->PIO_ABSR |= PIO_PB12B_PWMH0;   // Peripheral B for PB12

    PWM->PWM_CH_NUM[1].PWM_CMR = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;

    // Set period and duty cycle
    PWM->PWM_CH_NUM[1].PWM_CPRD = PWM_CPRD_CPRD(50);     // 50 µs channel period
    PWM->PWM_CH_NUM[1].PWM_CDTY = PWM_CDTY_CDTY(0);      // 0 ms channel duty cycle → Motor stopped

    // Activate channel
    PWM->PWM_ENA = PWM_ENA_CHID1;
}
*/
void pwm_set_duty_cycle(uint16_t duty_cycle){
    if (duty_cycle < 1000){
        duty_cycle = 1000;
    }
    else if (duty_cycle >  2000){
        duty_cycle = 2000;
    }
    REG_PWM_CDTYUPD1 = duty_cycle; // Oppdaterer duty cycle til channel 0
    PWM->PWM_SCUC = 1; //oppdatere duty cycle

    
    //uint32_t current_duty = REG_PWM_CDTY1;
    //printf("Current hardware duty cycle: %lu\n\r", current_duty);
    //PWM->PWM_CH_NUM[0].PWM_CDTY=duty_cycle;
    //printf("DUTY CYCLE: %u\n\r", duty_cycle);
}

void pwm_duty(CAN_MESSAGE *msg){ //kall noe annet- omgjør output fra can til duty cycle

    uint16_t duty=msg->data[0]*6 + 900; //0-200-> 900-2100
    if (duty>2100){
        duty=2100;
    }
    else if (duty < 900){
        duty=900;
    }

    pwm_set_duty_cycle(duty);
    printf("duty: %u\n\r", duty);
    //duty cycle: et sted mellom 900 og 2100

}


