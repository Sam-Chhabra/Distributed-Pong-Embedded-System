#include "spi.h"



void spi_init(){
    //pb3 and pb4 as output, SS1, SS2
    DDRB |= (1 << DISP_SS2) | (1 << IO_SS1)|(1<< MCP_SS);
    PORTB |= (1 << DISP_SS2) | (1 << IO_SS1)|(1<< MCP_SS);
    //MOSI: PB5 output
    DDRB |= (1 << MOSI);
    //sck: pb7 output
    DDRB |= (1 << SCK);
    //miso: pb6 input
    DDRB &= ~(1 << MISO);
    //spi mode 0: CPOL=0 and CPHA=0
    SPCR &= ~((1<<CPOL)|(1<<CPHA));
    // SPI enable, master, clk/16
    SPCR |= (1 << SPE) | (1 <<MSTR) | ( 1 << SPR0);
    //SPSR = (0<<SPI2X);
}


void spi_write(uint8_t byte){ //transmit
    SPDR = byte; //sender
    while (! (SPSR & (1 << SPIF))); //wait until done
}

uint8_t spi_txrx(uint8_t data){
    SPDR = data;
    while(!(SPSR & (1<<SPIF))); //wait
    return SPDR; //retunrn byte
}

uint8_t spi_read(){
    //send dummy byte to generate puls and to receive from slave
    SPDR = 0xFF;
    while(!(SPSR & (1<<SPIF))); //wait
    return SPDR; 
}

void spi_transferBytes(const uint8_t *tx_data, uint8_t *rx_data, uint8_t len){ 
    for (uint8_t i=0;i< len; i++){ 
        //if 0x00, send dummy byte, if not cero use tx_data[i] else send 0xff
        uint8_t send_byte = tx_data ? tx_data[i] : 0xFF; 
        //send byten
        SPDR = send_byte;
        while (!(SPSR & (1 << SPIF))); 
        if (rx_data){ 
            rx_data[i]=SPDR; //read data, if we want to send (not read): rx_data is null. 
        }
    }
}

void spi_selectSlave(uint8_t ss){
    PORTB |= (1 << IO_SS1) | (1 << DISP_SS2) | (1 << MCP_SS); //active low-> set all high
    PORTB &= ~(1 << ss); // low on the slave we want
}

void spi_deselectSlave(){
    PORTB |= (1 << IO_SS1) | (1 << DISP_SS2) | (1 << MCP_SS);
}
