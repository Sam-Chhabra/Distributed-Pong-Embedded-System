#include "oled.h"
#include "fonts.h"
#include "spi.h"


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
            OLED_write_data(0x0);
        }
    }
}


void OLED_home(){
    OLED_pos(0,0);
}


void OLED_goto_line(uint8_t line){
    uint8_t line2 = (0xb0)|(line); 
    OLED_write_cmd(line2);
}

void OLED_goto_column(uint8_t column){
    uint8_t lower = 0b00001111 & column;
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
    //choose ss2- CS low, cs low as long as we write
    spi_selectSlave(DISP_SS2);
    spi_write(cmd);
    spi_deselectSlave(); //deselect slave 2
}

void OLED_write_data(uint8_t data) 
{
    //DC =1
    PORTB |=(1<<DC_BIT);
    spi_selectSlave(DISP_SS2);
    _delay_ms(1);
    spi_write(data); 
    spi_deselectSlave(); //deselect slave 2
}

void OLED_print_char(char letter){
    
    uint8_t col=0;
    const unsigned char selected_font = font4;
    if (letter>=32 && letter <=127){
    //hente listen
        const unsigned char* liste = selected_font + (letter - 32) * 4; 
        for (int c=0;c<4;c++){ 
            uint8_t data = pgm_read_byte(&liste[c]);
            OLED_write_data(data);
            col++;
        }
    }
}

void OLED_print(char* word){
    
    while(*word != '\0'){
        OLED_print_char(*word++);
    }
}

