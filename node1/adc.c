#include "adc.h"
#define ADC_BASE 0x1000


void adc_init(void) {
    // Sett PD5 (OC1A) som output
    DDRD |= (1 << PD5);

    // Fast PWM 8-bit: WGM13:0 = 0b0101
    TCCR1A |= (1 << WGM10);
    TCCR1B |= (1 << WGM12);

    // Non-inverting mode på OC1A
    TCCR1A |= (1 << COM1A1);
    TCCR1A &= ~(1 << COM1A0);

    // Velg prescaler: clk/1 (full system clock)
    TCCR1B |= (1 << CS10);

    // Sett duty cycle til 50 %
    OCR1A = 127;

    // Sett PB0 som input uten pull-up
    DDRB &= ~(1 << DDB0);
    PORTB &= ~(1 << PORTB0);
}


adc_values_t adc_read_raw(){
    volatile uint8_t *ADC_DATA = (volatile uint8_t*)ADC_BASE;
    ADC_DATA[0]=0;
    //_delay_ms(20);

    adc_values_t values;
    values.joystick_y = ADC_DATA[0];
    values.joystick_x = ADC_DATA[1];
    values.slider_y   = ADC_DATA[3];
    values.slider_x   = ADC_DATA[2];

    //printf("x: %d\n", values.joystick_x);
    //printf("y: %d\n", values.joystick_y);
    //rintf("x2: %d\n", values.slider_x);
    //printf("y2: %d\n", values.slider_y);
    
    return values;
}


adc_values_t position(int16_t center_x, int16_t center_y) {
    adc_values_t values = adc_read_raw(); // henter joystick- og slider-verdier
    adc_values_t pos;

    // Konverter joystick Y

    if (values.joystick_y >= center_y) {  // positiv retning
        pos.joystick_y = ((int16_t)(values.joystick_y - center_y) * 100) / (248 - center_y);
    } else {  // negativ retning
        pos.joystick_y = -((int16_t)(center_y - values.joystick_y) * 100) / (center_y - 67);
    }

    // Konverter joy
    adc_values_t data;
    if (values.joystick_x >= center_x) {  // positiv retning
        pos.joystick_x = ((int16_t)(values.joystick_x - center_x) * 100) / (245 - center_x);
    } else {  // negativ retning
        pos.joystick_x = -((int16_t)(center_x - values.joystick_x) * 100) / (center_x - 71);
    }

    pos.slider_x = values.slider_x;
    pos.slider_y = values.slider_y;

    return pos;
}

volatile joy_direction get_joystickdirection(adc_values_t pos, int16_t center_x, int16_t center_y) {

    if (pos.joystick_y > 50) {
    return UP;
    }
    else if (pos.joystick_y < -50) {

        return DOWN;
    }
    else if (pos.joystick_x > 50) {
        return RIGHT;
    }
    else if (pos.joystick_x <-50) {
        return LEFT;
    }
    else {
        return NEUTRAL;
    }

}


adc_values_t pos_calibrate(){
    int16_t x=0;
    int16_t y=0;
    int16_t slider_x=0;
    int32_t slider_y=0;
    adc_values_t values;
    for (int i=0;i<100;i++){
        values = adc_read_raw();
        _delay_ms(10);
        x += values.joystick_x;
        y += values.joystick_y; 
        slider_x += values.slider_x;   
        slider_y += values.slider_y;  
        //printf("x: %d\n", values.joystick_x);
        //printf("y: %d\n", values.joystick_y);
        //printf("x2: %d\n", values.slider_x);
        //printf("y2: %d\n", values.slider_y);    
    }
    _delay_ms(10);
    adc_values_t data;
    data.joystick_x=x/100;
    data.joystick_y=y/100;
    data.slider_x=slider_x/100;
    data.slider_y=slider_y/100;
    _delay_ms(10);


    //printf("x: %d\n", data.joystick_x);
    //printf("y: %d\n", data.joystick_y);
    //printf("slid_x: %d\n", data.slider_x);
    //printf("slid_y: %d\n", data.slider_y);

    return data;
}

//uint8_t pos_read(void){
    //return adc_read(0);  // Joystick X
//}


