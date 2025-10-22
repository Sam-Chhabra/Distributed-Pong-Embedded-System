#include "mcp2515.h"

uint8_t mcp2515_init(){
    DDRB  |= (1 << MCP_SS);
PORTB |= (1 << MCP_SS);

    // 2) Keep PB4 (hardware SS) OUTPUT + HIGH so AVR stays SPI master
DDRB  |= (1 << PB4);
PORTB |= (1 << PB4);

    // 3) Deassert other slaves' CS (example: OLED on PB3)
DDRB  |= (1 << PB3);
PORTB |= (1 << PB3);
    uint8_t value;
    spi_init(); //?
    mcp2515_reset();
    _delay_ms(10);
      value = mcp2515_read(MCP_CANSTAT);
    printf("CANSTAT after reset: 0x%02X\n", value);  // should be 0x8x
    if ((value & MODE_MASK) != MODE_CONFIG){
        printf("MCP2515 is NOT in configuration mode after reset!\n");
        return 1;
    }
    return 0;
    //more initialization
}
uint8_t mcp2515_read(uint8_t address){
    //lowering cs pin
    PORTB &= ~(1 << MCP_SS);
    //read instruction: 0x03
    spi_write(MCP_READ);
    spi_write(address);
    uint8_t data= spi_read();
    //higering cs pin
    PORTB |= (1 << MCP_SS);
    //printf("%d read data\n",data);
    return data;
}
void mcp2515_write(uint8_t address, uint8_t data){
    PORTB &= ~(1 << MCP_SS);
    spi_write(MCP_WRITE);
    spi_write(address);
    spi_write(data);
    PORTB |= (1 << MCP_SS);
    //printf("%d write data: \n", data);
}

void mcp2515_request_to_send(int buffer_index){
    if (buffer_index>2){return;}
    PORTB &= ~(1 << MCP_SS);
    uint8_t data;
    if (buffer_index==0){
        data=MCP_TX0;
    }
    else if(buffer_index==1){
        data=MCP_TX1;
    }
    else if (buffer_index==2){
        data=MCP_TX2;
    }
    spi_write(data);
    PORTB |= (1 << MCP_SS);

}
void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data){
    PORTB &= ~(1 << MCP_SS);
    spi_write(MCP_BIT_MODIFY);
    spi_write(address);
    spi_write(mask);
    spi_write(data);
    PORTB |= (1 << MCP_SS);

}
void mcp2515_reset(){
    PORTB &= ~(1 << MCP_SS);
    spi_write(MCP_RESET); 
    PORTB |= (1 << MCP_SS);
}
uint8_t mcp2515_read_status(){
    PORTB &= ~(1 << MCP_SS);
    uint8_t status;
    spi_write(MCP_READ_STATUS);
    status = spi_read();
    PORTB |= (1 << MCP_SS);
    return status;
}


void mcp2515_set_mode(uint8_t mode) {
	mcp2515_bit_modify(MCP_CANCTRL, 0b11100000, mode);
    
    //while ( (mcp2515_read(MCP_CANSTAT) & MODE_MASK) != mode){printf("her"); }
    }
