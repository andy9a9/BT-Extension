#include "gl_dte.h"

void writeMemory(unsigned char adr, unsigned char data) {
   write_program_memory(MEMORY_PREFIX(NO_EEPROM)+adr, data, 1);
   //write_program_memory(0x1000+adr, data, 1);
}

int16 readMemory(unsigned char adr) {
   int16 data=0;
   read_program_memory(MEMORY_PREFIX(NO_EEPROM)+adr, data, 1);
   return data;
}   