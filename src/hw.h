#ifndef HW_H
#define HW_H

//USART PINS
#ifndef USART_PINS
#define USART_PINS
   #define USART_TX  PIN_C4
   #define USART_RX  PIN_C5
#endif

// signalizacna LED
#define LED          PIN_A5

// rele
//#byte PORTA = 0x0C
#byte LATA = 0x10C
//#bit LATA4 = LATA.4

#define RELAY_PIN    PIN_A4
#define RELAY(a)     (a)==0?output_low(RELAY_PIN):output_high(RELAY_PIN)
//#define RELAY_STATE  input_state(RELAY_PIN) // precitam stav relatka bez zmeny IN/OUT, ako to robi input(PIN_X)
#bit RELAY_STATE = LATA.4

// tlacidlo/AT mod
#define BUTTON       input(PIN_C3)
#define KEY          PIN_C2

void initHW(void);
void ledBlinkOK(void);
void ledBlinkERR(unsigned char off=0);
void setATmode(unsigned char state);

#include "usart.h"
#include "hw.c"
#include "flash.h"
#include "bt.h"

#endif   /* HW_H */
