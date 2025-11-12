#include "menu.h"
#include "adc.h"
#include "oled.h"
//pb 0 = joy btn

#define MENU_LINE_COUNT 4
#define MENU_ARROW_COLUMN 110

static void menu_draw_cursor(uint8_t active_line){
    for (uint8_t line = 0; line < MENU_LINE_COUNT; line++){
        OLED_pos(line, MENU_ARROW_COLUMN);
        if (line == active_line){
            OLED_print_char('<');
        } else {
            OLED_print_char(' ');
        }
    }
}


void menu_init(){
    DDRB &= ~(1 << JOY_BTN); //input
    OLED_home();
    OLED_print("line 1");

    OLED_pos(1,0);
    OLED_print("line 2");

    OLED_pos(2,0);
    OLED_print("line 3");

    OLED_pos(3,0);
    OLED_print("line 4");
}

uint8_t navigation(uint8_t x, uint8_t y){
    adc_values_t pos_joystick;
    joy_direction joy_dir;
    uint8_t menu_pos=0; 
    uint8_t max=3; //want four lines
    uint8_t min=0; //at least 0 lines

    menu_draw_cursor(menu_pos);

    while ((PINB & (1 << JOY_BTN))) { //while the btn doesnt get clicked

        get_position(x, y,&pos_joystick);
        _delay_ms(10);
        joy_dir = get_joystickdirection(&pos_joystick);
        _delay_ms(10);
        uint8_t moveable;

        if (joy_dir == NEUTRAL){
            moveable = 1;
        }
        if (moveable){
            if (joy_dir == DOWN) {
                if (menu_pos < max) {menu_pos++;}
                moveable = 0;
                menu_draw_cursor(menu_pos);
            }
            else if (joy_dir == UP) {
                if (menu_pos > min) {menu_pos--;}
                moveable = 0;
                menu_draw_cursor(menu_pos);
            }
        }
    }

    return menu_pos;
}
