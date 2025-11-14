#include "can.h"
#include "spi.h"
#include "mcp2515.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

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


void CAN_init_loopback(void)
{
    mcp2515_init();
    _delay_ms(100);
    //mcp2515_reset();
    
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



void can_normal_init(void){
    mcp2515_init();
    _delay_ms(100);
    uint8_t CNF1 = ((SJW)<<6)|(BRP);
    uint8_t CNF2 = (1<<7)|((phaseseg1)<<3)|((propag)<<0);
    uint8_t CNF3 = ((phaseseg2)<<0);

    mcp2515_write(MCP_CNF3, CNF3);
    mcp2515_write(MCP_CNF2, CNF2);
    mcp2515_write(MCP_CNF1, CNF1);
    char interrupt_mask = 0b00111111;
    mcp2515_write(MCP_CANINTE, interrupt_mask);
    mcp2515_set_mode(MODE_NORMAL);

}


void CAN_int_init_PD2(void)
{
    DDRD  &= ~(1 << PD2);
    PORTD |=  (1 << PD2);          
    MCUCR |=  (1 << ISC01);        
    GICR  |=  (1 << INT0);
    sei();
}



void can_send(const can_message* msg, uint8_t buffer_n)
{
    uint8_t offset = 0x10 * buffer_n;
    //send id 
    mcp2515_write(MCP_TXB0SIDH+offset, (uint8_t)(msg->id >> 3)); //upper 8 bits
    mcp2515_write(MCP_TXB0SIDL+offset, (uint8_t)(msg->id <<5));

    //send data length
    uint8_t length = msg->data_length & 0xF; //can only send 8 bytes
    mcp2515_write(MCP_TXB0DLC + offset, length);

    //write data 
    for (uint8_t i=0;i<length; i++){
        if (i>= 8) break;
        mcp2515_write(MCP_TXB0D0 + offset + i, msg->data[i]);
    }
    mcp2515_request_to_send(buffer_n);   
}

void can_read(can_message *message, uint8_t rx_buffer_n){
    //printf("trying to read buffer RD %D \n", rx_buffer_n);
    spi_selectSlave(MCP_SS);
    spi_write(MCP_READ_RX0 + 0x4 * rx_buffer_n); // initiate read of RX buffer n

    // Lese id (fra bit 0 til 16)
    uint8_t sidh = spi_read();
    uint8_t sidl = spi_read();

    message->id = ((uint16_t) sidh << 3)|(sidh >> 5); // ???

    // Utvidet ID (fra bit 16 til 32)
    spi_read();
    spi_read();

    // Lese lengde
    message->data_length = spi_read() & 0xF; // ???

    // Lese data
    for (uint8_t i = 0; i < message->data_length; i++){
    message->data[i] = spi_read();
    }

    message->data[message->data_length] = '\0'; // ???
    spi_deselectSlave();
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

void can_reset(){
    spi_selectSlave(MCP_SS);
    spi_write(MCP_RESET);
    spi_deselectSlave();
}