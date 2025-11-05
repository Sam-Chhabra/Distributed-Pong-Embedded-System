
#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <util/delay.h>

typedef struct {
    int16_t joystick_x;  // uint16_t kan bli negative verdier??
    int16_t joystick_y;
    int16_t slider_x;
    int16_t slider_y;
} adc_values_t;

typedef enum {
    LEFT,
    RIGHT,
    UP,
    DOWN,
    NEUTRAL
} joy_direction;

void adc_init(void);
void adc_read_raw(adc_values_t *values); //volatile
void position(int16_t center_x, int16_t center_y, adc_values_t *pos);
volatile joy_direction get_joystickdirection(adc_values_t *pos, int16_t center_x, int16_t center_y);
//void pos_calibrate();
//uint8_t pos_read(void);

void pos_calibrate(adc_values_t *data);

void send_joystick_pos(adc_values_t *cal_data);
uint8_t get_data_from_joystick(adc_values_t *cal_data);

void send_pos(adc_values_t *pos);