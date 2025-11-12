#include "adc.h"
#include "can.h"
#include "user_io_board.h"
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


void adc_read_raw(adc_values_t *values){
    volatile uint8_t *ADC_DATA = (volatile uint8_t*)ADC_BASE;
    ADC_DATA[0]=0;

    values->joystick_y = ADC_DATA[0];
    values->joystick_x = ADC_DATA[1];
    values->slider_y   = ADC_DATA[3];
    values->slider_x   = ADC_DATA[2];
}

void get_position(int16_t center_x, int16_t center_y, adc_values_t *pos) {
    adc_values_t v;
    adc_read_raw(&v);

    if (v.joystick_y >= center_y) {
        int16_t denom = (int16_t)(248 - center_y);
        if (denom < 1) denom = 1;
            pos->joystick_y = ((int16_t)(v.joystick_y - center_y) * 100) / denom;
    }
    else {
        int16_t denom = (int16_t)(center_y - 67);
        if (denom < 1) denom = 1;
            pos->joystick_y = -((int16_t)(center_y - v.joystick_y) * 100) / denom;
    }

    if (v.joystick_x >= center_x) {
        int16_t denom = (int16_t)(245 - center_x);
        if (denom < 1) denom = 1;
            pos->joystick_x = ((int16_t)(v.joystick_x - center_x) * 100) / denom;
    } 
    else {
        int16_t denom = (int16_t)(center_x - 71);
        if (denom < 1) denom = 1;
            pos->joystick_x = -((int16_t)(center_x - v.joystick_x) * 100) / denom;
    }

    pos->slider_x = v.slider_x;
    pos->slider_y = v.slider_y;

    if (pos->joystick_x > 100) pos->joystick_x = 100;
    if (pos->joystick_x < -100) pos->joystick_x = -100;
    if (pos->joystick_y > 100) pos->joystick_y = 100;
    if (pos->joystick_y < -100) pos->joystick_y = -100;
}

volatile joy_direction get_joystickdirection(adc_values_t *pos) {
    if (pos->joystick_y > 50) {
        return UP;
    }
    else if (pos->joystick_y < -50) { 
        return DOWN;
    }

    else if (pos->joystick_x > 50) { 
        return RIGHT;
    }
    else if (pos->joystick_x < -50) { 
        return LEFT;
    }
    return NEUTRAL;
}

void pos_calibrate(adc_values_t *data){
    int16_t x=0;
    int16_t y=0;
    int16_t slider_x=0;
    int32_t slider_y=0;
    adc_values_t values;
    for (int i=0;i<100;i++){
        adc_read_raw(&values);
        _delay_ms(10);
        x += values.joystick_x;
        y += values.joystick_y; 
        slider_x += values.slider_x;   
        slider_y += values.slider_y;  

    }
    _delay_ms(10);
    data->joystick_x=x/100;
    data->joystick_y=y/100;
    data->slider_x=slider_x/100;
    data->slider_y=slider_y/100;
    _delay_ms(10);
}

void send_joystick_pos(adc_values_t *cal_data){
    adc_values_t pos;
    get_position(cal_data->joystick_x,cal_data->joystick_y,&pos);
    _delay_ms(10);
    can_message    message_to_node2 = {
            .id = 0x00,
            .data_length = 3,
            .data = {pos.joystick_x+100, pos.joystick_y+100}
        };
    if (send_btn_R5()){
        message_to_node2.data[2]=1;
    }
    else{
        message_to_node2.data[2]=0;
    }
    

    can_send(&message_to_node2,0);

}

uint8_t send_btn_R5(){
    Buttons btn;
    if(!user_io_read_buttons(&btn)){//leser knapper
        _delay_ms(100);
        if(btn.R5){
            return 1;
       
        //while(btn.R5);
        }
    }
    return 0;
}
