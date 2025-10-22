#include "oled.h"


void OLED_init() // PDF:example appended to the datasheet
{ 
    DDRB|=(1<<DC_BIT); //dc bit
    OLED_write_cmd(0xae); //display off
    OLED_write_cmd(0xa1); // segment remapping 
    OLED_write_cmd(0xc8); // utgangsskannretning: com63~com0
    OLED_write_cmd(0x20); 
    OLED_write_cmd(0x02); //page adressing mode
    OLED_write_cmd(0xaf); // // Display ON

    OLED_clear_screen();
    OLED_home();
}

void OLED_clear_screen(){
    OLED_home();
    for (uint8_t l=0;l<8;l++){
        OLED_goto_line(l);

        for (uint8_t c=0;c<128;c++){
            //spi_selectSlave(DISP_SS2);
            //OLED_goto_column(c);
            OLED_write_data(0x0);
        }
    }
}


void OLED_home(){
    OLED_pos(0,0);
}


void OLED_goto_line(uint8_t line){
    uint8_t line2 = (0xb0)|(line); //?
    OLED_write_cmd(line2);
}

void OLED_goto_column(uint8_t column){

    //ifølge databladet må kolonne deles i to, oppe og nede
    //uint8_t lower = (0x00)| (column & 0x0F); //ved å ta column med operasjon OG med 0000 1111 får vi de fire nederste.
    uint8_t lower = 0b00001111 & column;
    //tar så eller med det som står i databladet
    //uint8_t upper = (0x10) | ((column>>4) & 0xF0); //tilsvarende over: OG med 1111 0000
    uint8_t upper = 0b00010000 | ((column>> 4) & 0b00001111);
    OLED_write_cmd(lower);
    OLED_write_cmd(upper);
}

void OLED_clear_line(uint8_t line){
    OLED_goto_line(line);
    OLED_goto_column(0);

    for (int i=0;i<128;i++){
        OLED_goto_column(i);
        OLED_write_data(0);
    }
}


void OLED_pos(uint8_t row, uint8_t column){
    OLED_goto_line(row);
    OLED_goto_column(column);
}

void OLED_write_cmd(uint8_t cmd){
    PORTB &= ~(1<<DC_BIT); //dc=0
    //velg ss2- CS low, cs er lav så lenge vi skriver
    spi_selectSlave(DISP_SS2);
    spi_write(cmd); //skriver data
    spi_deselectSlave(); //deselect slave 2
}

void OLED_write_data(uint8_t data) //volatile - slaven skriver
{
    //DC må være 1
    PORTB |=(1<<DC_BIT);
    //velg ss2- CS low, cs er lav så lenge vi skriver
    spi_selectSlave(DISP_SS2);
    _delay_ms(1);
    spi_write(data); //skriver data
    spi_deselectSlave(); //deselect slave 2
}

void OLED_print_char(char letter){
    
    uint8_t col=0;
    //OLED_pos(0,col);
    const unsigned char selected_font = font4;
    if (letter>=32 && letter <=127){
    //hente listen
        const unsigned char* liste = selected_font + (letter - 32) * 4; 
        for (int c=0;c<4;c++){ //går gjennom alle fire strengene i lista
            uint8_t data = pgm_read_byte(&liste[c]);
            OLED_write_data(data);
            col++;
            //OLED_pos(0,col);
        }
    }
}

void OLED_print(char* word){
    
    while(*word != '\0'){
        OLED_print_char(*word++);
    }
}

void OLED_set_brightness(uint8_t lvl);




/*OLED_write_cmd(0xa1); // segment remapping
OLED_write_cmd(0xda); // vanlige pad-hardware: alternativ
OLED_write_cmd(0x12);
OLED_write_cmd(0xc8); // utgangsskannretning: com63~com0
OLED_write_cmd(0xa8); // multipleksforhold: 63
OLED_write_cmd(0x3f);
OLED_write_cmd(0xd5); // visningsdelerforhold / oscillasjonsfrekvens-modus
OLED_write_cmd(0x80);
OLED_write_cmd(0x81); // kontrastkontroll
OLED_write_cmd(0x50);
OLED_write_cmd(0xd9); // forhåndsladningsperiode
OLED_write_cmd(0x21);

OLED_write_cmd(0xdb); // VCOM-frakoblingsnivåmodus
OLED_write_cmd(0x30);
OLED_write_cmd(0xad); // hovedkonfigurasjon
OLED_write_cmd(0x00);
OLED_write_cmd(0xa4); // utdata følger RAM-innhold
OLED_write_cmd(0xa6); // sett normal visning
OLED_write_cmd(0xaf); // skjerm på}

//segment remap
OLED_write_cmd(0xA1);
//scan direction
OLED_write_cmd(0xC8);
//output enable
OLED_write_cmd(0xAF);

OLED_write_cmd(0x20); // sett minneadresseringsmodus
OLED_write_cmd(0x02);
//OLED_write_cmd(0xae); // skjerm av
}*/