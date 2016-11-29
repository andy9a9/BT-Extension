#ifndef SYS_H
#define SYS_H

#include <16LF1454.h>
#ifdef __DEBUG
   #device ICD=TRUE
#endif
#FUSES NOWDT                 	//No Watch Dog Timer
#FUSES NOPUT                 	//No Power Up Timer
#FUSES MCLR                  	//Master Clear pin enabled
#FUSES NOPROTECT             	//Code not protected from reading
#FUSES NOBROWNOUT            	//No brownout reset
#FUSES NOCLKOUT              	//I/O function on OSC2
#FUSES IESO                  	//Internal External Switch Over mode enabled
#FUSES NOFCMEN               	//Fail-safe clock monitor disabled
#FUSES NOWRT                 	//Program memory not write protected
#FUSES NOSTVREN              	//Stack full/underflow will not cause reset
#FUSES BORV19                	//Brownout reset at 1.9V
#FUSES NOLPBOR               	//Low-Power Brownout reset is disabled
#FUSES NOLVP                 	//No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O

#use delay(internal=8MHz)
//#use rs232(baud=9600,parity=N,xmit=PIN_C4,rcv=PIN_C5,bits=8,stream=PORT1)

void setupClock(void);

#include "sys.c"

#endif   /* SYS_H */
