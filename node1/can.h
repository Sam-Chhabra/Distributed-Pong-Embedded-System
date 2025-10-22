#pragma once
#include <stdint.h>

typedef struct {
    uint16_t id;          // 11-bit standard ID
    uint8_t  data_length; // 0..8
    uint8_t  data[8];
} can_message;

void can_init(void);          
void CAN_int_init_both(void);  
void CAN_service(void);        


uint8_t can_try_send(const can_message* msg);
void can_data_receive(can_message* out);
