
#include "../sam/sam3x/include/sam.h"
#include "../sam/sam3x/include/sam3x8e.h"

#include <stdio.h>
#include "PWM.h"

//velger metode2
//REG_PWM_CPDRUPDx setter periode verdi

void pwm_init() {
    PMC->PMC_PCER0 |= PMC_PCER0_PID12;   // paralell io controller b
    PMC->PMC_PCER1 |= PMC_PCER1_PID36;   // PWM controller

    PIOB->PIO_PDR |= PIO_PDR_P13; //slår av gpio slik at pinnen kan brukes av pwm
    PIOB->PIO_ABSR |= PIO_PB13B_PWMH1;   //

    // Set up PWM clock
    PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(84);
    // f_PWMCLK = MCK / DIVA = 84 MHz / 84 = 1 MHz

    // CLKA channel mode, left-aligned (default)
    PWM->PWM_CH_NUM[1].PWM_CMR = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;

    /*
     * CPRD  = f_PWMCLK / f_signal
     * CDTY  = CPRD × duty
     */

    // Set period and duty cycle
    PWM->PWM_CH_NUM[1].PWM_CPRD = PWM_CPRD_CPRD(20000);  // 20 ms channel period
    PWM->PWM_CH_NUM[1].PWM_CDTY = PWM_CDTY_CDTY(1500);   // 1.5 ms channel duty cycle

    // Activate channel
    PWM->PWM_ENA = PWM_ENA_CHID1;

    /////////////////////////////////////////////
    // INIT MOTOR PWM
    /////////////////////////////////////////////

    // Deactivate PIO for PB12
    PIOB->PIO_PDR |= PIO_PDR_P12;
    PIOB->PIO_ABSR |= PIO_PB12B_PWMH0;   // Peripheral B for PB12

    PWM->PWM_CH_NUM[0].PWM_CMR = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;

    // Set period and duty cycle
    PWM->PWM_CH_NUM[0].PWM_CPRD = PWM_CPRD_CPRD(50);     // 50 µs channel period
    PWM->PWM_CH_NUM[0].PWM_CDTY = PWM_CDTY_CDTY(0);      // 0 ms channel duty cycle → Motor stopped

    // Activate channel
    PWM->PWM_ENA = PWM_ENA_CHID0;
}

void pwm_set_duty_cycle(uint16_t duty_cycle){
    if (duty_cycle<900){
        duty_cycle =900;
    }
    else if (duty_cycle>2100){
        duty_cycle=2100;
    }
    REG_PWM_CDTYUPD0 = duty_cycle; //
    PWM->PWM_SCUC =1; //oppdatere duty cycle
    uint32_t current_duty = REG_PWM_CDTY0;
    printf("Current hardware duty cycle: %lu\n\r", current_duty);
    PWM->PWM_CH_NUM[1].PWM_CDTY=duty_cycle;
    //printf("DUTY CYCLE: %u\n\r", duty_cycle);
}

void duty(CAN_MESSAGE *msg){ //kall noe annet- omgjør output fra can til duty cycle
    if (msg->data[0]=='1'){
        pwm_set_duty_cycle(900); //0.9 ms
    }
    else if (msg->data[0] == '0'){ //1.5 ms
        pwm_set_duty_cycle(1500);
    }
    else if (msg->data[0]=='2'){
        pwm_set_duty_cycle(2100);
    }
}



