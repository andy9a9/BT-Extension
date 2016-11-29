#include "gl_dte.h"
#include "bt.h"

void initBT(void) {
   memset(&btTXbuffer,0x00,sizeof(btTXbuffer));
   memset(&btRXbuffer,0x00,sizeof(btRXbuffer));

   // vynulovanie stavu iniclizacie
   btInitStatus=0x00;
   // nastavim dlzku odpovede
   btDataCount=0;

   // aktivujem AT mod
   setATmode(1);
   
   // zistenie pritomnosti modulu
   btTXbuffer[0]='A';
   btTXbuffer[1]='T';
   btTXbuffer[2]='\r';
   btTXbuffer[3]='\n';
   putsUSART(btTXbuffer,4);
   //fprintf(BTdev, "AT\r\n");
   
   // ? mensi delay, aby sa prikaz stihol prijat
   
   // zapni meranie timeoutu
   set_timer0(0);
   clear_interrupt(INT_TIMER0);
   enable_interrupts(INT_TIMER0);

   // cakaj na odpoved
   while(!btInitStatus && tmtMsg<BT_MAX_INIT_TIME);

   delay_ms(1);
   
   // deaktivujem AT mod
   setATmode(0);
   
   // odosli RESET
   //strcpy(btTXbuffer,BT_RESET_CMD);
   //sprintf(btTXbuffer,"%s\r\n",BT_RESET_CMD);
   //strcat(btTXbuffer,"a");
   //putsUSART(btTXbuffer,10);
   
   //while(kbhit(BTdev)) fgetc(BTdev);
}

int cmpInitBT(char *data) {
   unsigned char ret = 0;
   char tmpData[6] = {};
   char *respond = tmpData;
   
   // uloz odpoved OK
   strcpy(respond, BT_RESPOND_OK);
   // porovnaj ci, zaciatocny retazec je OK
   int i;
   for (i = 0; i<2; i++) {
      if (*(data + i) != *(respond + i)) ret = 1;
   }
   if (!ret) return 0;

   // uloz odpoved pre ERR
   strcpy(respond, BT_RESPOND_ERR);
   ret = 0;
   // porovnaj ci, zaciatocny retazec je ERROR
   for (i = 0; i<5; i++) {
      if (*(data + i) == *(respond + i)) ret++;
   }

   return ret;
}

int getCmdBT(void) {
   // zisti, ci sprava obsahuje header
   if (btRXbuffer[0]!=BT_IDENT) return 1;
   
   // skontroluj vypocitanu Ks s prijatou
   Ks=0;
   for(int i=1;i<BT_MSG_COUNT-2;i++) Ks+=btRXbuffer[i];
   if(btRXbuffer[BT_MSG_COUNT-2]!=((Ks>>8)&0xFF)) return 2;
   if(btRXbuffer[BT_MSG_COUNT-1]!=(Ks&0xFF)) return 2;

   int ret=0; // navratova hodnota
   unsigned char *tmpData;
   tmpData=btRXbuffer;
   tmpData+=2;
   
   // identifikuj prikaz
   switch(btRXbuffer[1]) {
      case BT_CMD_NAME:
         sysState=MODE_SERVICE;
         ret=setNameBT(tmpData);
         break;
      case BT_CMD_FIND:
         ledBlinkOK();
         break;
      case BT_CMD_STATE: // prikaz pre zapnutie/vypnutie
         if(btRXbuffer[2]==BT_CMD_ON) cmdState=1;
         else if(btRXbuffer[2]==BT_CMD_OFF) cmdState=0;
         else ret=3;
         break;
      case BT_CMD_TIME:
         ret=setTimeBT(tmpData);
         break;
      default:
         ret=3;
   }
   
   // zmazem prijimaci buffer
   memset(btRXbuffer, 0, sizeof(btRXbuffer));
   return ret;
}

int setNameBT(unsigned char *newName) {
   // uloz si meno do buffera
   unsigned char btName[19];
   unsigned char *fullName;
   fullName=btName;
   // vyskladaj retazec
   strcpy(fullName,BT_NAME_CMD);
   strcat(fullName,newName);
   // pridaj <cr><lf>
   btName[18]=0x00;
   btName[16]='\r';
   btName[17]='\n';
   
   // uloz meno do EEPROM
   unsigned char adr=2;
   do {
      write_EEPROM(adr,*newName);
      adr++;
   } while(newName++);
   
   // aktivuj AT mod
   setATmode(1);

   // odosli meno do BT 10(AT+NAME=...\r\n)
   putsUSART(fullName,10+BT_NAME_LENGTH);
   //printf("%s", fullName);
   
   // povol potvrdenie
   btInitStatus=1;
   tmtMsg=0;
   
   // zapni meranie timeoutu
   set_timer0(0);
   clear_interrupt(INT_TIMER0);
   enable_interrupts(INT_TIMER0);

   // cakaj na odpoved
   while(btInitStatus==1 && tmtMsg<BT_MAX_INIT_TIME);

   // deaktivujem AT mod
   setATmode(0);

   return tmtMsg;
}

int setTimeBT(unsigned char *newTime) {
   // overnie, ci ide o casovy vypinaci prikaz
   if(*(newTime)!=BT_CMD_OFF) return 3;

   // ulozim prijate data pre cas v tvare hod, min, sec
   memset(&glStTime,0x00,sizeof(glStTime));
   glStTime.hod=*(++newTime);
   glStTime.min=*(++newTime);
   glStTime.sec=*(++newTime);

   // aktivujem casovy rezim
   sysState=MODE_TIME;

   // spustim odpocitavanie casu
   set_timer2(0);
   clear_interrupt(INT_TIMER2);
   enable_interrupts(INT_TIMER2);

   return 0;
}

void sendRespondBT(unsigned char respond) {
   // kvituj prikaz
   if(respond) btTXbuffer[0]=BT_CMD_ERR;
   else btTXbuffer[0]=~BT_IDENT;

   // odosli odpvoed
   putcUSART(btTXbuffer[0]);
   //putc(btTXbuffer[0]);
   
   // zmazem odosielaci buffer
   memset(btTXBuffer,0x00,sizeof(btTXbuffer));
}