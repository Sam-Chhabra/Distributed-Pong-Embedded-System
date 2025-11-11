#include "adc.h"
#include "can.h"
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
    //_delay_ms(20);

    values->joystick_y = ADC_DATA[0];
    values->joystick_x = ADC_DATA[1];
    values->slider_y   = ADC_DATA[3];
    values->slider_x   = ADC_DATA[2];

    //printf("x: %d\n", values.joystick_x);
    //printf("y: %d\n", values.joystick_y);
    //rintf("x2: %d\n", values.slider_x);
    //printf("y2: %d\n", values.slider_y);
}

void position(int16_t center_x, int16_t center_y, adc_values_t *pos) {
adc_values_t v;
adc_read_raw(&v);

if (v.joystick_y >= center_y) {
    int16_t denom = (int16_t)(248 - center_y);
if (denom < 1) denom = 1;
    pos->joystick_y = ((int16_t)(v.joystick_y - center_y) * 100) / denom;
} else {
int16_t denom = (int16_t)(center_y - 67);
if (denom < 1) denom = 1;
pos->joystick_y = -((int16_t)(center_y - v.joystick_y) * 100) / denom;
}

if (v.joystick_x >= center_x) {
int16_t denom = (int16_t)(245 - center_x);
if (denom < 1) denom = 1;
pos->joystick_x = ((int16_t)(v.joystick_x - center_x) * 100) / denom;
} else {
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

volatile joy_direction get_joystickdirection(adc_values_t *pos, int16_t center_x, int16_t center_y) {

(void)center_x; (void)center_y;
const int8_t DEADZONE = 15;

if (pos->joystick_y > 50) {
 printf("UP");
return UP;}
else if (pos->joystick_y < -50) { printf("down");return DOWN;}

else if (pos->joystick_x > 50) { printf("right");return RIGHT;}
else if (pos->joystick_x < -50) { printf("left");return LEFT;}

if (pos->joystick_y > DEADZONE) return UP;
if (pos->joystick_y < -DEADZONE) return DOWN;
if (pos->joystick_x > DEADZONE) return RIGHT;
if (pos->joystick_x < -DEADZONE) return LEFT;

return NEUTRAL;
}



char get_joystickdirection1(adc_values_t *pos, int16_t center_x, int16_t center_y) {

(void)center_x; (void)center_y;
const int8_t DEADZONE = 15;

if (pos->joystick_y > 50) {
 
return '0x03';}
else if (pos->joystick_y < -50) { return '0x04';}

else if (pos->joystick_x > 50) { return '0x02';}
else if (pos->joystick_x < -50) { return '0x01';}


return '0x00';

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
        //printf("x: %d\n", values.joystick_x);
        //printf("y: %d\n", values.joystick_y);
        //printf("x2: %d\n", values.slider_x);
        //printf("y2: %d\n", values.slider_y);    
    }
    _delay_ms(10);
    data->joystick_x=x/100;
    data->joystick_y=y/100;
    data->slider_x=slider_x/100;
    data->slider_y=slider_y/100;
    _delay_ms(10);

    
    //printf("x: %d\n", data->joystick_x);
    //_delay_ms(100);
    //printf("y: %d\n", data->joystick_y);
    //_delay_ms(100);
    //printf("slid_x: %d\n", data->slider_x);
    //_delay_ms(100);
    //printf("slid_y: %d\n", data->slider_y);

}

//uint8_t pos_read(void){
    //return adc_read(0);  // Joystick X
//}
//adc_values_t pos_calibrate()
void send_joystick_pos(adc_values_t *cal_data){
    //adc_values_t cal_data = pos_calibrate(); //kalibrerer
    //#define MODE_NORMAL   0x00
    //mcp2515_set_mode(0x00); //normal mode (setter cnf-reg inni her)

adc_values_t pos;
    position(cal_data->joystick_x,cal_data->joystick_y,&pos);
    //printf("JOY X=%d Y=%d ", (int)pos.joystick_x, (int)pos.joystick_y);
    _delay_ms(10);
    char joy_data = get_joystickdirection1(&pos, cal_data->joystick_x, cal_data->joystick_y);


    can_message message_to_node2 = {
            .id = 0x43,
            .data_length = 1,
            .data[0] = joy_data
        };
    can_send(&message_to_node2,0);
     printf("JOY: %c \n\r",message_to_node2.data[0]);
    _delay_ms(100);
  
}
//joy_direction get_joystickdirection(adc_values_t pos, int16_t center_x, int16_t center_y)

void send_joystick_pos_ext(adc_values_t *cal_data){
    //adc_values_t cal_data = pos_calibrate(); //kalibrerer
    //#define MODE_NORMAL   0x00
    //mcp2515_set_mode(0x00); //normal mode (setter cnf-reg inni her)

    adc_values_t pos;
    position(cal_data->joystick_x,cal_data->joystick_y,&pos);
    //printf("JOY X=%d Y=%d ", (int)pos.joystick_x, (int)pos.joystick_y);
    _delay_ms(10);

    can_message message_to_node2 = {
            .id = 0x43,
            .data_length = 2,
            .data = {pos.joystick_x+100, pos.joystick_y+100}
        };
    can_send(&message_to_node2,0);
    printf("DATA node1: %d\n\r", message_to_node2.data[0]);
    printf("DATA node1: %d\n\r", message_to_node2.data[1]);

}