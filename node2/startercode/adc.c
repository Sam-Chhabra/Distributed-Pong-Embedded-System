#include "adc.h"
#include "../sam/sam3x/include/sam.h"
#include "../sam/sam3x/include/sam3x8e.h"

#include <stdio.h>


void adc_init(){
    	//PMC->PMC_PCR &= ~0x3F;					  // Clear the 6 first bits
        PMC->PMC_PCER1 |= PMC_PCER1_PID37;  //slår på ting vi vil bruke, adc har id 37
        ADC->ADC_MR |= ADC_MR_FREERUN_ON; //single ended freerun mode
        ADC->ADC_MR |= ADC_MR_PRESCAL(41); //1MHzt = 84M/((prescaler+1)*2)
        ADC->ADC_CHER |= ADC_CHER_CH2; //enable channel 2
        //ADC->ADC_MR |= ADC_MR_TRANSFER(0);
        //ADC->ADC_MR |= ADC_MR_TRACKTIM(2); 
        ADC->ADC_CR |= ADC_CR_START; //begin adc
}

uint32_t adc_data(){ //adc fra ir
    uint32_t data= ADC->ADC_CDR[2]; //ad2
    //uint32_t latestData = ADC->ADC_LCDR;
    return data;
}



