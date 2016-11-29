#ifndef USART_H
#define USART_H

#use rs232(baud=9600,parity=N,xmit=USART_TX,rcv=USART_RX,bits=8,stream=BTdev)

void putsUSART(char *data, char len);
void putcUSART(char data);
char getsUSART(void);

#include "usart.c"

#endif   /* USART_H */
