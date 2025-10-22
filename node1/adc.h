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
adc_values_t adc_read_raw(); //volatile
adc_values_t position(int16_t center_x, int16_t center_y);
volatile joy_direction get_joystickdirection(adc_values_t pos, int16_t center_x, int16_t center_y);
//void pos_calibrate();
//uint8_t pos_read(void);

adc_values_t pos_calibrate();