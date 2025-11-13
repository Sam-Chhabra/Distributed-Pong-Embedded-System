#include "ir.h"
#include "adc.h"

#include "../sam/sam3x/include/sam.h"
#include "../sam/sam3x/include/sam3x8e.h"
#include "../can_node_2/can_controller.h"

#include <stdio.h>

/**/
uint8_t ir_blocked(){ //Create a function that is able to count the score. This will later be used for the game application
    uint16_t data = adc_data(); //fra 0 -> 4091
    if (data<1000){ //count++
        return 1;
    }
    return 0;
}

void ir_count_score(uint8_t *score, Timer *time){ //flag 0 betyr at den ikke har scora på en stund
    if (ir_blocked() && (end_timer(time))){
        (*score)++;
        start_timer(time, seconds(2));
        //send mld til node1
        CAN_MESSAGE message;
        message.id=0x02;
        message.data_length = 1;
        message.data[0]=1;
        
        can_send(&message, 1);


        //delay_ms(1000000);
        printf("score: %u\n\r",(unsigned int)*score);
    }   
}
