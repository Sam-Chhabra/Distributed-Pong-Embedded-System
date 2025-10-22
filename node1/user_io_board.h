#pragma once
#include <stdint.h>

typedef struct __attribute__((packed)) {
  union { uint8_t right; struct { uint8_t R1:1,R2:1,R3:1,R4:1,R5:1,R6:1; }; };
  union { uint8_t left;  struct { uint8_t L1:1,L2:1,L3:1,L4:1,L5:1,L6:1,L7:1; }; };
  union { uint8_t nav;   struct { uint8_t NB:1,NR:1,ND:1,NL:1,NU:1;}; };
} Buttons;

void user_io_init(void);
int  user_io_read_buttons(Buttons *b);

// optional: handy for diagnosing byte order without touching SPI internals in main
void user_io_debug_raw(uint8_t *b0, uint8_t *b1, uint8_t *b2);
