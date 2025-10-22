#include "menu.h"
#include "adc.h"

//pb 0 = joy btn

#define MENU_LINE_COUNT 4
#define MENU_ARROW_COLUMN 110

static void menu_draw_cursor(uint8_t active_line){
    for (uint8_t line = 0; line < MENU_LINE_COUNT; line++){
        //OLED_goto_line(line);
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
    uint8_t menu_pos=0; //hvor starter vi
    uint8_t max=3; //fire linjer, ENDRE
    uint8_t min=0; //minst 0 linjer, ENDRE
    //pos_joystick = position(x,y);
    //joy_dir = get_joystickdirection(pos_joystick, x,y);
    menu_draw_cursor(menu_pos);

    while ((PINB & (1 << JOY_BTN))) { // kjør mens knappen IKKE trykkes

        pos_joystick = position(x, y);
        _delay_ms(10);
        joy_dir = get_joystickdirection(pos_joystick, x, y);
        _delay_ms(10);
        uint8_t moveable;

        if (joy_dir == NEUTRAL){
            moveable = 1;
        }
        if (moveable){
            if (joy_dir == DOWN) {
                printf("opp\n");
                _delay_ms(10);
                if (menu_pos < max) {menu_pos++;}
                
                printf("menu_pos: %d ", menu_pos);
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


/*    while (PINB & (1 << PB0)) { // kjør mens knappen IKKE trykkes
        pos_joystick = position(x, y);
        joy_dir = get_joystickdirection(pos_joystick, x, y);
        printf("inni løkken");

        if (joy_dir == UP) {
            printf("opp\n");
            if (menu_pos < max) menu_pos++;
            while (get_joystickdirection(position(x, y), x, y) == UP); // vent til sluppet
            _delay_ms(100);
        }
        else if (joy_dir == DOWN) {
            printf("ned\n");
            if (menu_pos > min) menu_pos--;
 uint16_t timeout = 0;
while (get_joystickdirection(position(x, y), x, y) == DOWN) {
    _delay_ms(10);
    timeout++;
    if (timeout > 200) break;  // etter 2 sekunder, gi opp
}
        }
        else {
            //printf("nøytral\n");
        }
        _delay_ms(20);
    }*/