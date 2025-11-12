
#include "../sam/sam3x/include/sam.h"
#include "../sam/sam3x/include/sam3x8e.h"

#include <stdio.h>
#include "PWM.h"

//velger metode2
//REG_PWM_CPDRUPDx setter periode verdi

void servo_init(){  
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

/*void motor_init(){
    PIOC->PIO_ABSR |= PIO_PC25 | PIO_PC26;
    PIOC->PIO_PDR = PIO_PC25 | PIO_PC26;

    PMC->PMC_PCER1 |= 1 << (ID_TC6- 32); //enable pwm peripheral clock
    //PMC->PMC_PCER1 |= 1 << (ID_TC7- 32); //enable pwm peripheral clock
    //PMC->PMC_PCER1 |= 1 << (ID_TC8- 32); //enable pwm peripheral clock
    
    TC2->TC_BMR = TC_BMR_QDEN | TC_BMR_POSEN | TC_BMR_EDGPHA; 
    TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_XC0 | TC_CMR_ETRGEDG_RISING | TC_CMR_ABETRG;
    TC2->TC_CHANNEL[0].TC_CCR |= TC_CCR_CLKEN;
    TC2->TC_BCR = TC_BCR_SYNC;

}*/

void motor_init(){
    //pb12 enable/pwm
    PIOB->PIO_PDR |= PIO_PDR_P12; // slår av gpio slik at pinnen kan brukes av pwm
    PIOB->PIO_ABSR |= PIO_ABSR_P12;
    //pioc
    PIOC->PIO_PDR |= PIO_PDR_P13;
    PIOC->PIO_ABSR |= PIO_ABSR_P13;
    //pc23 phase/dir
PIOC->PIO_PER |= PIO_PC23;  // Enable PIO control
PIOC->PIO_OER |= PIO_PC23;  // Sett PC23 som output
    //25 and 26 ChA ChB , quad a and quad b
    PIOC->PIO_ABSR |= PIO_PC25 | PIO_PC26;
    PIOC->PIO_PDR |= PIO_PC25 | PIO_PC26;
    
    PMC->PMC_PCER1 |= 1 << (ID_TC6- 32);
    PIOC->PIO_SODR|=PIO_PB23; //clear
    PWM->PWM_CH_NUM[0].PWM_CMR = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL; // channel 0 er motor, skal bruke klokke A (CLKA)
    

    REG_TC2_BMR |= TC_BMR_QDEN | TC_BMR_POSEN;  //set quadrature decoder mde, position measurment on channel 0 and 1

    TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_XC0 ;//set xc0 as the selected clock,
    TC2->TC_CHANNEL[0].TC_CCR |= TC_CCR_CLKEN; //enable clock
    
    TC2->TC_BCR = TC_BCR_SYNC;

    //PMC->PMC_PCER0 |= PMC_PCER0_PID29; //tc2 har id 29

    // Set period and duty cycle
    PWM->PWM_CH_NUM[0].PWM_CPRD = PWM_CPRD_CPRD(20000);  // setter CPRD som gir riktig periode, CPRD*DICA/MCK skal bli perioden på 20 ms
    PWM->PWM_CH_NUM[0].PWM_CDTY = PWM_CDTY_CDTY(0);   // setter start duty cycle på channel 0, 1.5 ms 

    // Activate channel
    PWM->PWM_ENA = PWM_ENA_CHID0;   // aktverer klokken på kanal 0
    motor_calibrate();
}

//uint16_t data= ADC->ADC_CDR[2]; //ad2

volatile int32_t motor_read(){
    //uint32_t val = REG_TC0_CV0;
    //return val; 100-(motor_read()/14.05);
    uint32_t val =  TC2->TC_CHANNEL[0].TC_CV;
    return val ; 
}

volatile int32_t motor_read1(uint32_t min){
    //uint32_t val = REG_TC0_CV0;
    //return val; 100-(motor_read()/14.05);
    uint32_t val = min + TC2->TC_CHANNEL[0].TC_CV;
    return 100-(val/14.05) ; 
}
/*void pwm_init_motor(){ //  LAB DAG 8
    pwm_init_servo();
    PMC->PMC_PCER0 |= PMC_PCER0_PID12;   // paralell io controller b
    PMC->PMC_PCER1 |= PMC_PCER1_PID36;   // PWM controller

    // Deactivate PIO for PB12
    PIOB->PIO_PDR |= PIO_PDR_P12;
    PIOB->PIO_ABSR |= PIO_PB12B_PWMH0;   // Peripheral B for PB12

    PWM->PWM_CH_NUM[0].PWM_CMR = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;

    // Set period and duty cycle
    PWM->PWM_CH_NUM[0].PWM_CPRD = PWM_CPRD_CPRD(50);     // 50 µs channel period
    PWM->PWM_CH_NUM[0].PWM_CDTY = PWM_CDTY_CDTY(0);      // 0 ms channel duty cycle → Motor stopped

    // Activate channel
    PWM->PWM_ENA = PWM_ENA_CHID0;
}*/


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

uint16_t pwm_motor_pos(CAN_MESSAGE *msg){ //kall noe annet- omgjør output fra can til duty cycle
    //direction
    uint8_t dir;
    if (msg->data[0]>110){
        PIOC->PIO_SODR|=PIO_PC23; //set
    }
    else if(msg->data[0]<90){
        PIOC->PIO_CODR|=PIO_PC23; //clear
    }

    int16_t speed=((msg->data[0])-100)*100; 
//speed
    if (speed<0){
        speed=speed*(-1);
    }

    REG_PWM_CDTYUPD0 = speed; // Oppdaterer speed cycle til channel 0
    PWM->PWM_SCUC = 1; //oppdatere speed cycle

    return speed;
    //printf("duty_x: %u\n\r", duty);
    //duty cycle: et sted mellom 900 og 2100

}


void pwm_servo_pos(CAN_MESSAGE *msg){
    uint16_t duty=msg->data[1]*6 + 900; //0-200-> 900-2100

    if (duty < 1000){
        duty = 1000;
    }
    else if (duty >  2000){
        duty = 2000;
    }
    REG_PWM_CDTYUPD1 = duty; // Oppdaterer duty cycle til channel 1
    PWM->PWM_SCUC = 1; //oppdatere duty cycle

    //printf("duty_y: %u\n\r", duty);
    //duty cycle: et sted mellom 900 og 2100

}

void PI_regulator(CAN_MESSAGE *msg, Timer *timer){
    static double sum;   // static = husker verdien mellom kall
    if (end_timer(timer)){
        double kp=1;
        double ki=1;
        double T=1/1000; //1 sampling per millisek
        double ref= (msg->data[0]-100); //joystick -100->100
        double y = 100-(motor_read()/14.05);//måling fra motor 1->2812, -100 til 100
        if (y<-100){
            y = -100;
        }
        else if (y>100){
            y = 100;
        }
        
        double error = ref-y;
        sum+=(error*T);
        double u = (kp*error) + (T*ki*sum);
        
        pwm_motor_speed(u);
        printf("u: %lf\n\r", u);
        start_timer(timer, msecs(1));
    }

}

int16_t get_motor_r(CAN_MESSAGE *msg){
    int16_t ref=((msg->data[0])); 
    return ref;
}

void pwm_motor_speed(double u){ //kall noe annet- omgjør output fra can til duty cycle
    //direction sjekke om den er over eller under 0
    if (u<0){
        PIOC->PIO_CODR|=PIO_PC23; // venstre
        u = (-1)*u;  
    }
    else {
        PIOC->PIO_SODR|=PIO_PC23;  // høyre
    }

    uint32_t duty = u*100;

    // setter riktig pådrag    
    REG_PWM_CDTYUPD0 = duty; // Oppdaterer duty cycle til channel 0
    PWM->PWM_SCUC = 1; //oppdatere speed cycle


    

}


int32_t motor_calibrate(){
    //går til høyre
    PIOC->PIO_SODR|=PIO_PC23;
    REG_PWM_CDTYUPD0 = 8000; 
    Timer timer;
    start_timer(&timer, seconds(2));
    while(!end_timer(&timer)){}
    REG_PWM_CDTYUPD0 = 0; 
    int32_t min = motor_read(); //pos helt til høyre


    //går til venstre
    PIOC->PIO_CODR|=PIO_PC23; //clear
    REG_PWM_CDTYUPD0 = 8000; 
    start_timer(&timer, seconds(2));
    while(!end_timer(&timer)){}
    REG_PWM_CDTYUPD0 = 0; 
    int32_t max = motor_read(); //pos helt til venstre
    int32_t center = (max-min)/2;
    printf("max: %d \n\n", max);
    printf("min: %d \n\n", min);
    printf("center: %d \n\n", center);
    return min;
    

}