#include "can.h"
#include "mcp2515.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>


#ifndef MCP_CNF1
#define MCP_CNF1        0x2A
#endif
#ifndef MCP_CNF2
#define MCP_CNF2        0x29
#endif
#ifndef MCP_CNF3
#define MCP_CNF3        0x28
#endif
#ifndef MCP_CANINTE
#define MCP_CANINTE     0x2B
#endif
#ifndef MCP_CANINTF
#define MCP_CANINTF     0x2C
#endif
#ifndef MCP_CANCTRL
#define MCP_CANCTRL     0x0F
#endif

#ifndef MCP_RXB0CTRL
#define MCP_RXB0CTRL    0x60
#endif
#ifndef MCP_RXB1CTRL
#define MCP_RXB1CTRL    0x70
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

#ifndef MODE_LOOPBACK
#define MODE_LOOPBACK   0x40
#endif

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


static volatile uint8_t can_irq_flag = 0;
static volatile uint8_t rx_available = 0;
static can_message rx_last;


static inline void id_to_regs(uint16_t id, uint8_t* sidh, uint8_t* sidl){
    *sidh = (uint8_t)(id >> 3);
    *sidl = (uint8_t)((id & 0x07) << 5);
}
static inline uint16_t regs_to_id(uint8_t sidh, uint8_t sidl){
    return ((uint16_t)(sidh << 3)) | ((sidl >> 5) & 0x07);
}


static inline uint8_t MCP_get_interrupt_flags(void){
    return mcp2515_read(MCP_CANINTF);
}
static inline void MCP_clear_interrupt_flags(uint8_t mask){
    mcp2515_bit_modify(MCP_CANINTF, mask, 0);
}


static void mcp_abort_all_tx(void){
    mcp2515_bit_modify(MCP_CANCTRL, CANCTRL_ABAT, CANCTRL_ABAT);
    while ( (mcp2515_read(MCP_TXB0CTRL) & MCP_TXREQ_BIT) ||
            (mcp2515_read(MCP_TXB1CTRL) & MCP_TXREQ_BIT) ||
            (mcp2515_read(MCP_TXB2CTRL) & MCP_TXREQ_BIT) ) { }
    mcp2515_bit_modify(MCP_CANCTRL, CANCTRL_ABAT, 0x00);
}
static void mcp_flush_rx(void){
    uint8_t f = mcp2515_read(MCP_CANINTF);
    if (f & MCP_RX0IF){
        uint8_t dlc = mcp2515_read(MCP_RXB0DLC) & 0x0F;
        for (uint8_t i=0;i<dlc;i++) (void)mcp2515_read(MCP_RXB0D0 + i);
        mcp2515_bit_modify(MCP_CANINTF, MCP_RX0IF, 0);
    }
    if (f & MCP_RX1IF){
        uint8_t dlc = mcp2515_read(0x75) & 0x0F;   /* RXB1DLC */
        for (uint8_t i=0;i<dlc;i++) (void)mcp2515_read(0x76 + i); /* RXB1D0 */
        mcp2515_bit_modify(MCP_CANINTF, MCP_RX1IF, 0);
    }
}
static void mcp_zero_txb0(void){
    mcp2515_write(MCP_TXB0SIDH, 0x00);
    mcp2515_write(MCP_TXB0SIDL, 0x00);
    mcp2515_write(MCP_TXB0DLC,  0x00);
}


static void CAN_init_loopback(void)
{
    mcp2515_init();
    mcp2515_reset();

    mcp2515_write(MCP_CNF1, 0x03); 
    mcp2515_write(MCP_CNF2, 0x90);
    mcp2515_write(MCP_CNF3, 0x02);

    mcp2515_write(MCP_RXB0CTRL, 0x60);
    mcp2515_write(MCP_RXB1CTRL, 0x60);

    mcp2515_bit_modify(MCP_CANINTF, 0xFF, 0);     /* clear stale flags */
    mcp_abort_all_tx();                            /* abort pending TX */
    mcp_flush_rx();                                /* flush RX buffers */
    mcp_zero_txb0();

    mcp2515_write(MCP_CANINTE, MCP_RX0IF | MCP_TX0IF); /* enable RX0/TX0 IRQs */

    mcp2515_set_mode(MODE_LOOPBACK);
    mcp2515_bit_modify(MCP_CANINTF, 0xFF, 0);
}

void can_init(void){
    CAN_init_loopback();
}


void CAN_int_init_PD2(void)
{
    DDRD  &= ~(1 << PD2);
    PORTD |=  (1 << PD2);          
    MCUCR |=  (1 << ISC01);        
    GICR  |=  (1 << INT0);
    sei();
}


uint8_t can_try_send(const can_message* msg)
{
    if (!msg || msg->data_length > 8) return 0;
    if (mcp2515_read(MCP_TXB0CTRL) & MCP_TXREQ_BIT) return 0;

    uint8_t sidh, sidl;
    id_to_regs(msg->id, &sidh, &sidl);

    mcp2515_write(MCP_TXB0SIDH, sidh);
    mcp2515_write(MCP_TXB0SIDL, sidl);
    mcp2515_write(MCP_TXB0DLC,  (uint8_t)(msg->data_length & 0x0F));
    for (uint8_t i = 0; i < msg->data_length; i++)
        mcp2515_write((uint8_t)(MCP_TXB0D0 + i), msg->data[i]);

    mcp2515_request_to_send(0);  /* TXB0 */
    return 1;
}


static void can_read_rxb0(can_message* out)
{
    uint8_t sidh = mcp2515_read(MCP_RXB0SIDH);
    uint8_t sidl = mcp2515_read(MCP_RXB0SIDL);
    out->id = regs_to_id(sidh, sidl);

    uint8_t dlc = mcp2515_read(MCP_RXB0DLC) & 0x0F;
    out->data_length = dlc;

    for (uint8_t i = 0; i < dlc; i++)
        out->data[i] = mcp2515_read((uint8_t)(MCP_RXB0D0 + i));

    MCP_clear_interrupt_flags(MCP_RX0IF);
}


ISR(INT0_vect){ can_irq_flag = 1; }


void CAN_service(void)
{
    if (!can_irq_flag) return;
    can_irq_flag = 0;

    uint8_t flags = MCP_get_interrupt_flags();

    if (flags & MCP_RX0IF){
        can_message tmp;
        can_read_rxb0(&tmp);
        rx_last = tmp;
        rx_available = 1;
    }
    if (flags & MCP_TX0IF){
        MCP_clear_interrupt_flags(MCP_TX0IF);
    }
    if (flags & MCP_RX1IF){
        MCP_clear_interrupt_flags(MCP_RX1IF);
    }
}


uint8_t CAN_try_get(can_message* out){
    if (!rx_available) return 0;
    *out = rx_last;
    rx_available = 0;
    return 1;
}
