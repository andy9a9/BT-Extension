#include "gl_dte.h"

void putsUSART(char *data, char len) {
   for(int i=0;i<len;i++) {
      // postupne budem odosielat data
      putcUSART(*data++);
   }   
}

void putcUSART(char data) {
   fputc(data,BTdev);
}

char getcUSART(void) {
   return fgetc(BTdev);
}
