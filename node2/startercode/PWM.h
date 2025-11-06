
//#pragma once

#include <stdint.h>
#include "../can_node_2/can_controller.h"
void pwm_init();
void pwm_set_duty_cycle(uint16_t duty_cycle);

void duty(CAN_MESSAGE *msg);