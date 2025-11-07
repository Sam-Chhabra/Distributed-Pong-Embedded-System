
//#pragma once


#include <stdint.h>
#include "../can_node_2/can_controller.h"

#define PWM_CHANNEL 1

void pwm_init();
void pwm_set_duty_cycle(uint16_t duty_cycle);

void pwm_duty(CAN_MESSAGE *msg);