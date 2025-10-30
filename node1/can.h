#pragma once
#include <stdint.h>

typedef struct {
    uint16_t id;          // 11-bit standard ID
    uint8_t  data_length; // 0..8
    uint8_t  data[8];
} can_message;

/* Init MCP2515 in loopback, clean state, enable chip IRQs */
void can_init(void);

/* Enable PD2 (INT0) interrupt for MCP2515 INT (active-low) */
void CAN_int_init_PD2(void);

/* Call this in your main loop to handle RX/TX work signaled by ISR */
void CAN_service(void);

/* Non-blocking send: returns 1 if queued, 0 if TX buffer busy */
uint8_t can_try_send(const can_message* msg);

/* One-message mailbox: returns 1 if a new frame was available and copied into *out */
uint8_t CAN_try_get(can_message* out);
