
#include <stdint.h>

#ifndef MCP_CANCTRL
#define MCP_CANCTRL     0x0F
#endif

#ifndef MCP_TXB0CTRL
#define MCP_TXB0CTRL    0x30
#endif
#ifndef MCP_TXB1CTRL
#define MCP_TXB1CTRL    0x40
#endif
#ifndef MCP_TXB2CTRL
#define MCP_TXB2CTRL    0x50
#endif

#ifndef MCP_TXB0SIDH
#define MCP_TXB0SIDH    0x31
#endif
#ifndef MCP_TXB0SIDL
#define MCP_TXB0SIDL    0x32
#endif
#ifndef MCP_TXB0DLC
#define MCP_TXB0DLC     0x35
#endif
#ifndef MCP_TXB0D0
#define MCP_TXB0D0      0x36
#endif

#ifndef MCP_RXB0SIDH
#define MCP_RXB0SIDH    0x61
#endif
#ifndef MCP_RXB0SIDL
#define MCP_RXB0SIDL    0x62
#endif
#ifndef MCP_RXB0DLC
#define MCP_RXB0DLC     0x65
#endif
#ifndef MCP_RXB0D0
#define MCP_RXB0D0      0x66
#endif

#define MODE_LOOPBACK   0x40




#ifndef MCP_RX0IF
#define MCP_RX0IF 0x01
#endif
#ifndef MCP_RX1IF
#define MCP_RX1IF 0x02
#endif
#ifndef MCP_TX0IF
#define MCP_TX0IF 0x04
#endif
#ifndef MCP_TXREQ_BIT
#define MCP_TXREQ_BIT 0x08
#endif

#define CANCTRL_ABAT    0x10  



#define BIT_RATE  250000;
#define FOSC_MCP  16000000;
#define BRP   (FOSC_MCP / (2*BIT_RATE * 16)) -1;
#define phaseseg2 6-1;
#define phaseseg1   phaseseg2+1;
#define propag   2-1;
#define SJW   1-1;
#define smp 0;

typedef struct {
    uint16_t id;          // 11-bit standard ID
    uint8_t  data_length; // 0..8
    uint8_t  data[8];
} can_message;

/* Init MCP2515 in loopback, clean state, enable chip IRQs */
void can_normal_init(void);

/* Enable PD2 (INT0) interrupt for MCP2515 INT (active-low) */
void CAN_int_init_PD2(void);

/* Call this in your main loop to handle RX/TX work signaled by ISR */
void CAN_service(void);

/* Non-blocking send: returns 1 if queued, 0 if TX buffer busy */
uint8_t can_try_send(const can_message* msg, CAN_TX_BUFFER buffer);


/* One-message mailbox: returns 1 if a new frame was available and copied into *out */
uint8_t CAN_try_get(can_message* out);

void CAN_init_normal();

//void can_receive(can_message* message, CAN_RX_BUFFER buffer);