#include "sys.h"
#include "param.h"
#include "gl_dte.h"

// globalne premenne
volatile unsigned char tmtMsg;   // timeout na prichadzajucu spravu
unsigned char cmdState; // stav relatka
unsigned char sysState; // stav systemu
unsigned int16 Ks;      // kontrolna suma
unsigned char newCmd;   // flag, ze prisiel novy prikaz
struct stTime glStTime; // struktura na riadenie casoveho ovladania
unsigned char fiveTime; // cas pre 20ms casovac, 5x20=100ms
unsigned char tenTime;  // cas pre 100ms, 10x100ms=1s

// prototypy
int checkTime(void);

#INT_TIMER0
void TIMER0_isr() {
   // oznacim, ze vyprsal timeout na spravu
   tmtMsg++;
   // vymaz flag
   clear_interrupt(INT_TIMER0);
   // prijimaj znova
   btDataCount=0;
}

#INT_TIMER2
void TIMER2_isr() {
   // pocitaj cas, iba ak ide o casovy rezim
   if(sysState==MODE_TIME) {
      // odpocitaj 1 od od casu do vypnutia
      // pripocitaj pocet pre 20ms
      fiveTime++;
      // ak uz napoctal 100ms, zacni pocitat nasobky
      if(fiveTime==5) {
         fiveTime=0;
         tenTime++;
      }
      else if(tenTime==10) {
         tenTime=0;
         // skontroluj cas, ktory este zosatava do prepnutia, vypni odpocitavanie
         if(checkTime()) disable_interrupts(INT_TIMER2);
      }
      // vymaz priznak prerusenia
      clear_interrupt(INT_TIMER2);
   }
}
   
#INT_RDA
void RDA_isr(void) {
   //vunuluj timeout
   set_timer0(0);
   // preictaj jeden BYTE z USARTu
   btRXbuffer[btDataCount]=getcUSART();
  
   // ak nahodou bezi mod casovaca, tak ho prepni na normal
   if(sysState!=MODE_SERVICE) sysState=MODE_NORMAL;
   
   // ak ide o servisny mod
   if(sysState==MODE_SERVICE) { 
      // zistenie stavu inicializacie
      if(btInitStatus!=BT_INIT_STATUS) {
         int actMsgCount=BT_GET_INIT_COUNT(btInitStatus); 
         if(btDataCount<actMsgCount) btDataCount++;
         else {
            // vypni kontrolu timeoutu na prichadzajuci znak
            disable_interrupts(INT_TIMER0);
            // zakaz prerusenie
            disable_interrupts(INT_RDA);
            btDataCount=0;

            // ak nastala chyba inicializacie, blikaj ERR
            if(cmpInitBT(&btRXbuffer)) {
               ledBlinkERR(1);
               // nastavim priznak pre timeout
               tmtMsg=BT_MAX_INIT_TIME;
               return;
            }
            // posun inicializacny buffer
            btInitStatus++;
            // vynuluj priznak pre timeout
            tmtMsg=0;
            // povol prerusenie
            enable_interrupts(INT_RDA);
         }
      }
   }   
   else if (sysState==MODE_NORMAL) {
      // skontroluj maximalny pocet prijatych sprav
      if(btDataCount<BT_MSG_COUNT-1) {
         // povol kontrolu timeoutu
         btDataCount++;
         // vymaz flag
         clear_interrupt(INT_TIMER0);
         set_timer0(0);
         enable_interrupts(INT_TIMER0);
      }
      else {
         // vypni kontrolu timeoutu na prichadzajuci znak
         disable_interrupts(INT_TIMER0);
         // zakaz prerusenie
         disable_interrupts(INT_RDA);
         
         btDataCount=0;
         // oznam, ze si nieco prijal
         newCmd=MODE_NORMAL;
      }
   }
   // vymaz prijimaci flag
   clear_interrupt(INT_RDA);
}

int checkTime(void) {
   // kontrola, ci je este nutne pocitat cas
   if(!glStTime.hod && !glStTime.min && !glStTime.sec) {
      // vypni RELE
      cmdState=0;
      return 1;
   }

   // odoberaj cas postupne
   if(glStTime.sec) glStTime.sec--; // ak existuju sekundy, odober tie
   else {
      if(--glStTime.min) {  // ak neexistuju sekundy, odober minuty
         glStTime.sec=59; // nastav sekundy
      }
      else {
         if(--glStTime.hod) {  // ak neexistuju minuty, odober hodiny
            glStTime.min=59; // pridaj minuty
            glStTime.sec=59; // nastav sekundy
         }
      }
   }
   return 0;
}
  
void main(void) {
   // nastavenie oscilatora (interny, 8MHz)
   setupClock();
   // nastavenie IN/OUT
   initHW();
   
   // vynulovanie casovaca pre timeout na prichadzajuci znak
   set_timer0(0);
   disable_interrupts(INT_TIMER0);
   tmtMsg=0;
   /*
   unsigned char btName[8];
   memset(&btName,0,sizeof(btName));
   // ziskaj nazov z EEPROM
   for(int i=0;i<BT_NAME_LENGTH;i++)
      btName[i]=readEEPROM(2+i);

   // nazov nie je platny, zapis novy
   char *tmpName;
   if((btName[0]==0x00) || (btName[0]==0xFF)) {
      tmpName = btName;
      // zapis novy nazov
      strcpy(tmpName,BT_NAME);
      int btNameSize=strlen(tmpName);
      unsigned char adr=2;
      do {
         writeEEPROM(adr,*tmpName);
         adr++;
         tmpName++;
      } while(--btNameSize);
   }*/

   // povol globalne presuenia
   enable_interrupts(GLOBAL);

   // nastav mod na AT
   sysState=MODE_SERVICE;

   // vynuluj pritomnost noveho prikazu
   newCmd=0;
   
   // blikneme, ze zijeme
   ledBlinkOK();
   
   // mensi delay
   delay_ms(10);
   
   // povol prerusenie na prijem noveho znaku
   clear_interrupt(INT_RDA);
   enable_interrupts(INT_RDA);
   
   // inicializacia BT
   initBT();
   
   // vyprsal timeout pocas inicializacie
   if(tmtMsg) {
      //!!! docasne vyblokujem kontrolu inicializacie
      //while(TRUE) ledBlinkERR();
   }
   
    // nastav normalny mod
   sysState=MODE_NORMAL;
   cmdState=0;
   
   // vynuluj odpocitavaci cas
   fiveTime=0;
   tenTime=0;

   while(TRUE) {
      // kontroluj stav tlacidla
      if(!BUTTON) {
         delay_ms(10);
         while(!BUTTON);
         //prepni stav
         cmdState^=1;
      }

      // kontroluj novy prikaz
      if(newCmd&MODE_NORMAL) {
         if(tmtMsg) ledBlinkERR();
         else {
            ledBlinkOK();
            // spracuj prijaty retazec
            newCmd=getCmdBT();
            // kvituj prijaty retazec
            sendRespondBT(newCmd);
         }
         tmtMsg=0;
         newCmd=0;

         // povol prerusenie
         enable_interrupts(INT_RDA);
      }

      // kontroluj zmenu stavu
      if(cmdState!=RELAY_STATE) {
         RELAY(cmdState);
         sysState=MODE_NORMAL;
      }

      // aktivuje sleep mod
      if(sysState==MODE_SLEEP) {
         SLEEP();
         sysState=MODE_NORMAL;
      }
   }   
}   