#ifndef FLASH_H
#define FLASH_H

#define NO_EEPROM 1

#if NO_EEPROM
   #define write_EEPROM writeMemory
   #define writeEEPROM writeMemory
   
   #define read_EEPROM readMemory
   #define readEEPROM readMemory
#endif

#define MEMORY_PREFIX(a) a==0?0:0x1000

void writeMemory(unsigned char adr, unsigned char data);
int16 readMemory(unsigned char adr);

#include "flash.c"

#endif