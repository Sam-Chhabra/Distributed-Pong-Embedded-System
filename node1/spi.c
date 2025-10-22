#include "spi.h"


void spi_init(){
    //pb3 and pb4 as output, SS1, SS2
    DDRB |= (1 << DISP_SS2) | (1 << IO_SS1);
    PORTB |= (1 << DISP_SS2) | (1 << IO_SS1);
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
}


void spi_write(uint8_t byte){ //transmit
    SPDR = byte; //sender
    while (! (SPSR & (1 << SPIF))); //venter på at sendinga er ferdig
    //ingen returverdi?
}

uint8_t spi_txrx(uint8_t data){
    SPDR = data;
    while(!(SPSR & (1<<SPIF))); //venter til overføring er ferdig
    return SPDR; //retunerer motatt byte
}

uint8_t spi_read(){
    //må sende dummy-byte for å generere klokkepuls og motta data fra slave
    SPDR = 0xFF;
    while(!(SPSR & (1<<SPIF))); //venter til overføring er ferdig
    return SPDR; //retunerer motatt byte
}

void spi_transferBytes(const uint8_t *tx_data, uint8_t *rx_data, uint8_t len){ 
    for (uint8_t i=0;i< len; i++){ //går gjennom alle bytes'a
        uint8_t send_byte = tx_data ? tx_data[i] : 0xFF; //hvis null, sendes dummy-byte. if tx_data ikke er null, bruk tx[i], else: 0xff
        //send byten
        SPDR = send_byte;
        while (!(SPSR & (1 << SPIF))); //venter på at sendinga er ferdig
        if (rx_data){ //leser data- hvis vi skal sende skal vi ikke lese: rx_data=NULL (rx_data er der vi lagrer data)
            rx_data[i]=SPDR;
        }
    }
}

void spi_selectSlave(uint8_t ss){
    PORTB |= (1 << IO_SS1) | (1 << DISP_SS2) | (1 << MCP_SS); // | (1 << OTHER_SS); // setter alle høy fordi de er aktiv lav
    PORTB &= ~(1 << ss); // setter lav på den vi skal velge
}

void spi_deselectSlave(){
    PORTB |= (1 << IO_SS1) | (1 << DISP_SS2) | (1 << MCP_SS);
}
