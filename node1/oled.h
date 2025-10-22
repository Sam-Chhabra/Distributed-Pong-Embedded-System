#include <avr/pgmspace.h>
#include "spi.h"
#include <util/delay.h>
#include "fonts.h"

#define DC_BIT PB2

void OLED_init();
void OLED_home();
void OLED_goto_line(uint8_t line);
void OLED_goto_column(uint8_t column);
void OLED_clear_line(uint8_t line);
void OLED_pos(uint8_t row, uint8_t column);
void OLED_pos(uint8_t row, uint8_t column);
void OLED_write_data(uint8_t data);
void OLED_print_char(char letter);
void OLED_print(char* word);
void OLED_clear_screen();
void OLED_set_brightness(uint8_t lvl);
