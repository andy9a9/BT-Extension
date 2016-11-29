#include "gl_dte.h"

void initHW(void) {
   // vypnem SPI
   setup_spi(SPI_DISABLED);
   
  	//vypni vsetky vystupy
   output_a(0x00);
}
   
void ledBlinkOK(void) {
   //prepni LED
   output_high(LED);
   delay_ms(200);
   output_low(LED);
}

void ledBlinkERR(unsigned char off) {
   //prepni LED
   output_toggle(LED);
   delay_ms(100);
   if(off) output_low(LED);
}

void setATmode(unsigned char state) {
   if(state) {
      //set_tris_a(get_tris_a()&0xFB);
      output_high(KEY);
   }
   else {
      //set_tris_a(get_tris_a()|0x04);
      output_low(KEY);
   }
   delay_ms(5);
}
